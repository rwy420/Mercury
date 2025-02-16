#ifndef __MERCURY__MEMORY__GDT_H
#define __MERCURY__MEMORY__GDT_H

#include <common/types.h> 

#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x10
#define USER_CODE_SEGMENT 0x18
#define USER_DATA_SEGMENT 0x20
#define TSS_SEGMENT 0x28

typedef struct
{
	uint16_t size;
	uint32_t address;
} __attribute__((packed)) GDT;

typedef struct
{
    uint16_t limit_low;      // Lower 16 bits of the limit
    uint16_t base_low;       // Lower 16 bits of the base
    uint8_t base_middle;     // Middle 8 bits of the base
    uint8_t access;          // Access byte
    uint8_t granularity;     // High 4 bits of the limit and flags
    uint8_t base_high;       // Upper 8 bits of the base
} __attribute__((packed)) GDTEntry;

typedef struct {
	uint16_t previous_task, __previous_task_unused;
	uint32_t esp0;
	uint16_t ss0, __ss0_unused;
	uint32_t esp1;
	uint16_t ss1, __ss1_unused;
	uint32_t esp2;
	uint16_t ss2, __ss2_unused;
	uint32_t cr3;
	uint32_t eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	uint16_t es, __es_unused;
	uint16_t cs, __cs_unused;
	uint16_t ss, __ss_unused;
	uint16_t ds, __ds_unused;
	uint16_t fs, __fs_unused;
	uint16_t gs, __gs_unused;
	uint16_t ldt_selector, __ldt_sel_unused;
	uint16_t debug_flag, io_map;
} __attribute__((packed)) TSS;

void gdt_set_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void segments_install_gdt();

void segments_load_gdt(GDT gdt);
void segments_load_registers();

#endif 
