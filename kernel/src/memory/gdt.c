#include <memory/gdt.h>
#include <common/screen.h>
#include <memory/common.h>

GDTEntry gdt_entries[6];

GDT g_gdt;
TSS g_tss;

void gdt_set_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt_entries[index].base_low = (base & 0xFFFF);           // Lower 16 bits of the base
    gdt_entries[index].base_middle = (base >> 16) & 0xFF;    // Middle 8 bits of the base
    gdt_entries[index].base_high = (base >> 24) & 0xFF;      // Upper 8 bits of the base

    gdt_entries[index].limit_low = (limit & 0xFFFF);         // Lower 16 bits of the limit
    gdt_entries[index].granularity = ((limit >> 16) & 0x0F); // Upper 4 bits of the limit
    gdt_entries[index].granularity |= (flags & 0xF0);        // Set flags (granularity, size)

    gdt_entries[index].access = access;                      // Set the access byte
}

void segments_install_gdt()
{
	g_gdt.address = (uint32_t) gdt_entries;
	g_gdt.size = (sizeof(GDTEntry) * 6) - 1;

	memset(&g_tss, 0, sizeof(TSS));
	g_tss.ss0 = KERNEL_DATA_SEGMENT;

	gdt_set_entry(0, 0, 0, 0, 0);
	gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xCF);
	gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xCF);
	gdt_set_entry(3, 0, 0xFFFFF, 0x9A, 0xCF);
	gdt_set_entry(4, 0, 0xFFFFF, 0x93, 0xCF);
	gdt_set_entry(5, (uint32_t) &g_tss, sizeof(TSS), 0x89, 0x40);

	segments_load_gdt(g_gdt);
	segments_load_registers();

	g_tss.esp0 = 0x90000;
	g_tss.ss0 = 0x10;

	g_tss.cs = 0x08;
	g_tss.ss = g_tss.ds = g_tss.es = g_tss.fs = g_tss.gs = 0x10;
	asm("ltr %%ax" :: "a" ((uint16_t) TSS_SEGMENT));
}
