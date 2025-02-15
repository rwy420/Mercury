#ifndef __QS__MEMORY__GDT_H
#define __QS__MEMORY__GDT_H

#include <core/types.h> 

typedef struct
{
	uint16_t size;
	uint32_t address;
} __attribute__((packed)) GDT;

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t  base_mid;
	uint8_t  access;
	uint8_t  granularity;
	uint8_t  base_high;
} __attribute__((packed)) GDTEntry;

void gdt_set_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void segments_install_gdt();

void segments_load_gdt(GDT gdt);
void segments_load_registers();

#endif 
