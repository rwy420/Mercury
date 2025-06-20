#include <hardware/usb/xhci_controller.h>
#include <hardware/usb/usb.h>
#include <common/screen.h>

extern DeviceDescriptor* g_pci_devices;
extern uint8_t g_pci_num_devices;

void usb_init()
{
	for(int i = 0; i < g_pci_num_devices; i++)
	{
		DeviceDescriptor* device = &g_pci_devices[i];

		if(device->class_id == 0x0C && device->subclass_id == 0x03)
		{
			switch(device->prog_if)
			{
				case 0x30:
					printf("<USB> Found xHCI USB Controller\n");
					if(xhci_take_ownership(device))
						printf("<USB> Took ownership of xHCI Controller\n");
					else
						printf("<USB> Failed to take ownership of xHCI Controller\n");
					break;

				default:
					break;
			}
		}
	}
}

void usb_init_controller(DeviceDescriptor* device)
{
	switch(device->prog_if)
	{
		case 0x30:
			if(!xhci_create_controller(device))
				printf("<USB> Failed to initialize xHCI Controller\n");
			else
				printf("<USB> Initialized xHCI Controller\n");
			break;

		default:
			break;
	}
}
