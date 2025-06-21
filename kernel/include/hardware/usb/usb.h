#ifndef __MERCURY__HARDWARE__USB__USB_H
#define __MERCURY__HARDWARE__USB__USB_H

#include <hardware/pci.h>

typedef enum
{
	LOW_SPEED = 1,
	FULL_SPEED = 2,
	HIGH_SPEED = 3,
	SUPER_SPEED = 4
} USB_SPEED;

void usb_init();
void usb_init_controller(DeviceDescriptor* device);

USB_SPEED usb_speed_to_class(uint8_t speed_id);

#endif
