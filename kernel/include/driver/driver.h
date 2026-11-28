#ifndef __MERCURY__DRIVER__DRIVER_H
#define __MERCURY__DRIVER__DRIVER_H

#define MAX_DRIVER_NUM 0x10

#include <types.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>

typedef enum
{
	KEYBOARD,
	ATA,
	SATA,
	ETHERNET
} DRIVER_TYPE;

typedef struct Driver
{
	uint8_t id;
	const char* name;
	int enabled;
	DRIVER_TYPE type;
	DeviceDescriptor* device_descriptor;
	void (*init_handle)(struct Driver* self);
	void (*enable_handler)();
	void (*disbale_handler)();
	void* driver_interface;
} Driver;

uint8_t create_driver(const char* name, DRIVER_TYPE type, void* init_handle, void* enable_handle, void* disbale_handle, DeviceDescriptor* device_descriptor);
void set_interface(uint8_t driver, void* interface);
void* get_interface(uint8_t driver);
void enable_all_drivers();
void init_devices();
void enable_driver(uint8_t id);
void disable_driver(uint8_t id);

#endif
