#include <memory/gdt.h>
#include <core/screen.h>
#include <memory/common.h>

static GDTEntry gdt_entries[3];
GDT gdt;

void gdt_set_entry(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt_entries[index].base_low = base & 0xFFFF;
	gdt_entries[index].base_mid = base >> 16 & 0xFF;
	gdt_entries[index].base_high = base >> 24 & 0xFF;
	gdt_entries[index].limit_low = limit & 0xFFFF;
	gdt_entries[index].granularity = (flags & 0xF0) | (limit >> 16 & 0xF);
	gdt_entries[index].access = access;
}

void segments_install_gdt()
{
	gdt.address = (uint32_t) gdt_entries;
	gdt.size = (sizeof(GDTEntry) * 3) - 1;

	gdt_set_entry(0, 0, 0, 0, 0);
	gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xC0);
	gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xC0);

	segments_load_gdt(gdt);
	segments_load_registers();
}
