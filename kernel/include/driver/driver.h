#ifndef __MERCURY__DRIVER__DRIVER_H
#define __MERCURY__DRIVER__DRIVER_H

#include <common/types.h>
#include <hardware/interrupts.h>

typedef enum
{
	KEYBOARD,
	ATA,
	SATA
} DRIVER_TYPE;

typedef struct
{
	uint32_t interrupt;
	string name;
	DRIVER_TYPE type;
	void (*init_handle)();
	void (*enable_handler)();
	void (*disbale_handler)();
	bool enabled;
	uint8_t id;
} Driver;

void init_drivers();
uint8_t create_driver(string name, DRIVER_TYPE type, void* init_handle, void* enable_handle, void* disbale_handle);
void add_driver(Driver* driver);
void enable_driver(uint8_t id);
void disable_driver(uint8_t id);

#endif
