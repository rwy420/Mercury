#include "hardware/pci.h"
#include <hardware/usb/xhci_controller.h>
#include <hardware/usb/xhci_structs.h>
#include <memory/paging.h>
#include <common/screen.h>
#include <hardware/pit.h>

xHCIController xhci_controller;

int xhci_take_ownership(DeviceDescriptor* device)
{
	BAR* bar = device->bars[0];

	printf("xHCI BAR at: 0x");
	print_hex32(bar->address);
	printf(" with size of 0x");
	print_hex32(bar->size);
	printf("\n");

	for(uint32_t i = bar->address; i < bar->address + bar->size; i += PAGE_SIZE) map_page((void*) i, (void*) i);

	pci_enable_bus_mastering(device);
	pci_enable_memory(device);

	xHCICapabilityRegs* capabilities = (xHCICapabilityRegs*) bar->address;
	uint16_t extented_offset = capabilities->hccparams_1.xhci_extented_capability_pointer;

	for(int i = 0; i < 0x100; i += 0x10)
	{
		print_hex(i);
		printf(": ");
		for(int j = 0; j < 0x10; j++)
		{
			uint8_t offset = i + j;
			print_hex(((uint8_t*) bar->address)[offset]);
			printf(" ");
		}
		printf("\n");
	}

	uint16_t ext_off = *(uint16_t*) &capabilities + 0x10;

	extented_offset = ext_off;

	printf("EXT: ");
	print_hex32(ext_off);
	printf(" AT ADDRESS ");
	print_hex32((uint32_t) &capabilities->hccparams_1.xhci_extented_capability_pointer);
	printf("\n");

	if(extented_offset == 0x0)
	{
		printf("xHCI missing extented capabilities\n");
		return false;
	}



	uint32_t extented_address = bar->address + extented_offset * 4;

	while(true)
	{
		xHCIExtentedCap* extented_cap = (xHCIExtentedCap*) extented_address;

		if(extented_cap->capablity_id == USB_LEGACY_SUPPORT)
		{
			xHCILegacyUSBSupportCap* legacy = (xHCILegacyUSBSupportCap*) extented_address;
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
