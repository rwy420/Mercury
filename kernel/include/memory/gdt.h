#ifndef __MERCURY__MEMORY__GDT_H
#define __MERCURY__MEMORY__GDT_H

#include <core/types.h> 

struct GDT
{
	uint16_t size;
	uint32_t address;
} __attribute__((packed));

struct GDTDescriptor
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t limit_flags;
	uint8_t base_high;
} __attribute__((packed));

void gdt_init(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void segments_install_gdt();

void segments_load_gdt(struct GDT gdt);
void segments_load_registers();

#endif 
