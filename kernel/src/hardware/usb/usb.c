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

USB_SPEED usb_speed_to_class(uint8_t speed_id)
{
	switch(speed_id)
	{
		case 1: return LOW_SPEED;
		case 2: return FULL_SPEED;
		case 3: return HIGH_SPEED;
		case 4: return SUPER_SPEED;
	}

	return 0;
}

uint8_t usb_class_to_speed(USB_SPEED speed)
{
	switch(speed)
	{
		case LOW_SPEED: return 1;
		case FULL_SPEED: return 2;
		case HIGH_SPEED: return 3;
		case SUPER_SPEED: return 4;
	}

	return 0;
}
