#ifndef __MERCURY__HARDWARE__USB__USB_H
#define __MERCURY__HARDWARE__USB__USB_H

#include <hardware/pci.h>

typedef enum : uint8_t
{
	LOW_SPEED = 1,
	FULL_SPEED = 2,
	HIGH_SPEED = 3,
	SUPER_SPEED = 4
} USB_SPEED;

typedef struct
{
	uint8_t parent_port_id, slot_id, depth;
	USB_SPEED speed;
	uint32_t route;
} USBInfo; 

void usb_init();
void usb_init_controller(DeviceDescriptor* device);

USB_SPEED usb_speed_to_class(uint8_t speed_id);

#endif
