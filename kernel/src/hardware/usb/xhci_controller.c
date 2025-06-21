#include <hardware/usb/xhci_controller.h>
#include <hardware/usb/xhci_structs.h>
#include <memory/paging.h>
#include <memory/paging.h>
#include <common/screen.h>
#include <memory/common.h>
#include <hardware/pit.h>

xHCIController xhci_controller;
extern PageDirectory* g_kernel_pd;

int xhci_take_ownership(DeviceDescriptor* device)
{
	BAR* bar = device->bars[0];
	for(uint32_t i = bar->address; i < bar->address + bar->size * 2; i += PAGE_SIZE) map_page_pd_flags(g_kernel_pd, (void*) i, (void*) i, 
			PDE_PRESENT | PDE_RW | PDE_CACHE_DISABLE | PDE_WRITE_THROUGH);

	pci_enable_bus_mastering(device);
	pci_enable_memory(device);

	volatile xHCICapabilityRegs* capabilities = (volatile xHCICapabilityRegs*) bar->address;
	uint16_t extented_offset = capabilities->hccparams_1.xhci_extented_capability_pointer;

	if(extented_offset == 0x0)
	{
		return false;
	}

	uint32_t extented_address = bar->address + extented_offset * 4;

	while(true)
	{
		volatile xHCIExtentedCap* extented_cap = (volatile xHCIExtentedCap*) extented_address;

		if(extented_cap->capablity_id == USB_LEGACY_SUPPORT)
		{
			volatile xHCILegacyUSBSupportCap* legacy = (volatile xHCILegacyUSBSupportCap*) extented_address;
			if(!legacy->hc_bios_owned) return true;

			legacy->hc_os_owned = true;

			const uint32_t timeout = ms_since_init() + 1000;
			while(legacy->hc_bios_owned)
				if(ms_since_init() > timeout)
					return false;

			return true;
		}

		if(extented_cap->next_capability == 0) break;

		extented_address += extented_cap->next_capability;
	}

	return true;
}

int xhci_create_controller(DeviceDescriptor* device)
{
	// TODO

	if(xhci_init_controller(device))
		return true;
	else
		return false;
}

int xhci_init_controller(DeviceDescriptor* device)
{
	printf("<USB> Initializing xHCI Controller at ");
	print_hex(device->bus);
	printf(" ");
	print_hex(device->device_id);
	printf(" ");
	print_hex(device->function);
	printf("\n");

	BAR* bar = device->bars[0];

	xhci_controller.capability_regs = (volatile xHCICapabilityRegs*) bar->address;
	xhci_controller.operational_regs = (volatile xHCIOperationalRegs*) (bar->address + xhci_controller.capability_regs->cap_length);
	xhci_controller.runtime_regs = (volatile xHCIRuntimeRegs*) (bar->address + (xhci_controller.capability_regs->rstoff & ~0x1Fu));
	xhci_controller.bar0 = bar->address;

	volatile xHCICapabilityRegs* capabilities = xhci_controller.capability_regs;

	if(bar->type != MM) return false;
	if(!xhci_reset_controller()) return false;

	printf("<USB>   xHCI Version: ");
	print_hex(capabilities->hci_version >> 8);
	printf(".");
	print_hex(capabilities->hci_version & 0xFF);
	printf("\n");

	printf("<USB>   Max slots ");
	print_hex32(+capabilities->hcsparams_1.max_slots);
	printf("\n<USB>   Max intrs ");
	print_hex32(+capabilities->hcsparams_1.max_ints);
	printf("\n<USB>   Max ports ");
	print_hex32(+capabilities->hcsparams_1.max_ports);
	printf("\n");

	if(!xhci_init_ports()) return false;

	volatile xHCIOperationalRegs* operational = xhci_controller.operational_regs;
	xhci_controller.dcbaa_region = dma_create(capabilities->hcsparams_1.max_slots * 8);
	memset((void*) xhci_controller.dcbaa_region->phys, 0, xhci_controller.dcbaa_region->size);
	uint64_t dcbaap_phys = xhci_controller.dcbaa_region->phys;
	operational->dcbaap_lo = dcbaap_phys & 0xFFFFFFFF;
	operational->dcbaap_hi = dcbaap_phys >> 32;

	xhci_controller.command_ring_region = dma_create(0x100 * sizeof(xHCITRB));
	memset((void*) xhci_controller.command_ring_region->phys, 0, xhci_controller.command_ring_region->size);
	uint64_t command_ring_phsy = xhci_controller.command_ring_region->phys;
	operational->crcr_lo = command_ring_phsy | RING_CYCLE_STATE;
	operational->crcr_hi = command_ring_phsy >> 32;

	return true;
}

int xhci_init_ports()
{
	volatile xHCICapabilityRegs* capabilities  = xhci_controller.capability_regs;
	uint8_t max_ports = capabilities->hcsparams_1.max_ports;


	uint16_t extented_offset = capabilities->hccparams_1.xhci_extented_capability_pointer;
	uint32_t extented_address = xhci_controller.bar0 + extented_offset * 4;

	while(true)
	{
		const volatile xHCIExtentedCap* ext_cap = (volatile xHCIExtentedCap*) extented_address;

		if(ext_cap->capablity_id == SUPPORTED_PROTOCOL)
		{
			const volatile xHCISupportedProtocolCap* protocol = (volatile xHCISupportedProtocolCap*) ext_cap;
			const uint32_t protocol_name = ' BSU'; // "USB "

			if(protocol->name_string != protocol_name)
			{
				printf("<USB> Invalid protocol name\n");
				return false;
			}

			if(protocol->compatible_port_offset == 0 || protocol->compatible_port_offset + protocol->compatible_port_count -  1 > max_ports)
			{
				printf("<USB> Invalid ports\n");
				return false;
			}

			if(protocol->protocol_slot_type != 0)
			{
				printf("<USB> Invalid slot type\n");
				return false;
			}

			for(uint32_t i = 0; i < protocol->compatible_port_count; i++)
			{
				xHCIPort* port = &xhci_controller.ports[i];
				port->revision_major = protocol->major_version;
				port->revision_minor = protocol->minor_version;
			}
		}

		if(ext_cap->next_capability == 0) break;

		extented_address += ext_cap->next_capability * 4;
	}
	
	xhci_controller.operational_regs->config.max_device_slots_enabled = capabilities->hcsparams_1.max_slots;

	return true;
}

int xhci_reset_controller()
{
	volatile xHCIOperationalRegs* operational = xhci_controller.operational_regs;
	const uint32_t timeout = ms_since_init() + 500;

	while(operational->usbsts & CONTROLLER_NOT_READY)
		if(ms_since_init() > timeout)
			return false;

	operational->usb_cmd.host_controller_reset = 1;
	while(operational->usb_cmd.host_controller_reset)
		if(ms_since_init() > timeout)
			return false;

	return true;
}
