#include <hardware/usb/xhci.h>
#include <hardware/usb/xhci_structs.h>
#include <memory/paging.h>
#include <common/screen.h>

int xhci_take_ownership(DeviceDescriptor* device)
{
	BAR* bar = device->bars[0];
	map_page((void*) bar->address, (void*) bar->address);

	xHCICapabilityRegisters* capabilities = (xHCICapabilityRegisters*) bar->address;
	uint8_t extented_offset = capabilities->hccparams_1.xhci_extented_capability_pointer;

	if(extented_offset == 0x0)
	{
		return false;
	}

	uint32_t extented_address = bar->address + extented_offset * 4;
	while(true)
	{
		ExtentedCap* extented_cap = (ExtentedCap*) extented_address;

		if(extented_cap->capablity_id == USB_LEGACY_SUPPORT)
		{
			LegacyUSBSupportCap* legacy = (LegacyUSBSupportCap*) extented_address;
			if(!legacy->hc_bios_owned) return true;

			legacy->hc_os_owned = true;

			//TODO: Timeout timer to set ownership
			
			return true;
		}

		if(extented_cap->next_capability == 0) break;

		extented_address += extented_cap->next_capability;
	}

	return true;
}
