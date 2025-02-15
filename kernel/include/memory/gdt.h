#ifndef __QS__MEMORY__GDT_H
#define __QS__MEMORY__GDT_H

#include <core/types.h> 

struct GDT
{
	uint16_t size;
	uint32_t address;
} __attribute__((packed));

typedef struct 
{
	// 16 bits limit
	uint16_t limit_low;
	// 24 bits base 
	uint16_t base_low; 
	uint8_t base_middle;


	// 8 bits access flags
	// 1: accessed
	// 2: read_write
	// 3: conforming_expand_down
	// 4: code
	// 5: code_data_segment
	// 6, 7: DPL
	// 8: Present
	uint8_t access;
	// 8 bits limit flags
	// 1, 2, 3, 4: limit high
	// 5: available
	// 6: long mode
	// 7: big
	// 8: gran
	uint8_t limit_flags;

	// final 8 bits base => total of 32
	uint8_t base_high;
} __attribute__((packed)) GDTDescriptor;



void gdt_init(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void segments_install_gdt();

void segments_load_gdt(struct GDT gdt);
void segments_load_registers();
void flush_tss(void);

uint16_t gdt_code_segment_selector();
uint16_t gdt_data_segment_selector();

//TSS
typedef struct
{
	uint32_t prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
	uint32_t esp0;     // The stack pointer to load when changing to kernel mode.
	uint32_t ss0;      // The stack segment to load when changing to kernel mode.
	// Everything below here is unused.
	uint32_t esp1; // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap_base;
} __attribute__((packed)) TSSEntry;

void write_tss();
void tss_set_kernel_stack(uint32_t stack);

#endif 
