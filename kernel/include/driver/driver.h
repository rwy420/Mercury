#ifndef __MERCURY__DRIVER__DRIVER_H
#define __MERCURY__DRIVER__DRIVER_H

#include <hardware/interrupts.h>
#include <core/types.h>

typedef void (*enable_handler_t)();
typedef void (*disbale_handler_t)();

typedef void (*isr_t)(uint32_t interrupt);

enum DriverType
{
	KEYBOARD = 0,
	ATA = 1,
};

struct Driver
{
	uint32_t interrupt;
	string name;
	enum DriverType type;
	isr_t interrupt_handler;
	enable_handler_t enable_handler;
	disbale_handler_t disbale_handler;
	bool enabled;
	uint8_t id;
};

uint8_t create_driver(uint32_t interrupt, string name, enum DriverType type, isr_t interrupt_handler, 
		enable_handler_t enable_handler, disbale_handler_t disbale_handler);
void add_driver(struct Driver* driver);
void enable_driver(uint8_t id);
void disable_driver(uint8_t id);
void handle_driver_interrupt(uint8_t interrupt);

#endif
