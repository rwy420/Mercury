#ifndef __MERCURY__DRIVER__AM79C973__AM79C973_H
#define __MERCURY__DRIVER__AM79C973__AM79C973_H

#include <common/types.h>
#include <driver/driver.h>

typedef struct
{
	uint16_t mode;
	unsigned reserved1 : 4;
	unsigned num_send_buffers : 4;
	unsigned reserved2 : 4;
	unsigned num_receive_buffers : 4;
	uint64_t physical_address : 48;
	uint16_t reserved3;
	uint64_t logical_address;
	uint32_t receive_buffer_descriptor_address;
	uint32_t send_buffer_descriptor_address;
} __attribute__((packed)) __attribute__((aligned(16))) InitBlock;

typedef struct
{
	uint32_t address;
	uint32_t flags;
	uint32_t flags2;
	uint32_t available;
} __attribute__((packed))  BufferDescriptor;

void am79c973_init(Driver* self);
void am79c973_enable();
void am79c973_disable();
void am79c973_send(char* buffer, size_t size);
void am79c973_receive();
uint64_t am79c973_get_mac_address();
uint32_t am79c973_get_ip_address();

#endif
