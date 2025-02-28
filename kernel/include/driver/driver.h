#ifndef __MERCURY__DRIVER__DRIVER_H
#define __MERCURY__DRIVER__DRIVER_H

#include <common/types.h>
#include <hardware/interrupts.h>

typedef enum
{
	KEYBOARD,
	ATA,
	SATA,
	ETHERNET
} DRIVER_TYPE;

typedef struct
{
	void(*send)(char* buffer, size_t size);
	void(*receive)();
	uint64_t(*get_mac_address)();
	uint32_t(*get_ip_address)();
} EthernetInterface;

typedef struct
{
	uint8_t id;
	string name;
	bool enabled;
	DRIVER_TYPE type;
	void (*init_handle)(uint8_t id);
	void (*enable_handler)();
	void (*disbale_handler)();
	void* driver_interface;
} Driver;

void init_drivers();
uint8_t create_driver(string name, DRIVER_TYPE type, void* init_handle, void* enable_handle, void* disbale_handle);
void set_interface(uint8_t driver, void* interface);
void* get_interface(uint8_t driver);
void enable_all_drivers();
void enable_driver(uint8_t id);
void disable_driver(uint8_t id);

void net_send(char* buffer, size_t size);

#endif
