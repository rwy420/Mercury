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

typedef struct
{
	uint8_t request_type;
	uint8_t request;
	uint16_t value;
	uint16_t index;
	uint16_t length;
} USBRequestDescriptor;

typedef enum : uint8_t
{
	DEVICE_TO_HOST = 0b1 << 7,
	STANDART = 0b00 << 5,
	DEVICE = 0b00000
} USBRequestType;

typedef enum : uint8_t
{
	GET_DESCRIPTOR = 6,
	SET_DESCRIPTOR = 7
} USBRequest;

typedef enum : uint8_t
{
	DEVICE_DESCRIPTOR = 1
} USBRequestDescriptorType;

void usb_init();
void usb_init_controller(DeviceDescriptor* device);

USB_SPEED usb_speed_to_class(uint8_t speed_id);
uint8_t usb_class_to_speed(USB_SPEED speed);

#endif
