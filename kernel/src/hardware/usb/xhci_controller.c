#include "hardware/pci.h"
#include <hardware/usb/xhci_controller.h>
#include <hardware/usb/xhci_structs.h>
#include <memory/paging.h>
#include <common/screen.h>
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
		printf("xHCI missing extented capabilities\n");
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
	printf("<Mercury> Initializing xHCI Controller at ");
	print_hex(device->bus);
	printf(" ");
	print_hex(device->device_id);
	printf(" ");
	print_hex(device->function);
	printf("\n");

	BAR* bar = device->bars[0];



	xhci_controller.capability_regs = (xHCICapabilityRegs*) bar->address;
	xhci_controller.operational_regs = (xHCIOperationalRegs*) (bar->address + xhci_controller.capability_regs->cap_length);
	xhci_controller.runtime_regs = (xHCIRuntimeRegs*) (bar->address + (xhci_controller.capability_regs->rstoff & ~0x1Fu));


	if(bar->type != MM) return false;
	if(!xhci_reset_controller()) return false;

	printf("xHCI Version: ");
	print_hex(xhci_controller.capability_regs->hci_version >> 8);
	printf(".");
	print_hex(xhci_controller.capability_regs->hci_version & 0xFF);
	printf("\n");

	return true;
}

int xhci_reset_controller()
{
	xHCIOperationalRegs* operational = xhci_controller.operational_regs;
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
