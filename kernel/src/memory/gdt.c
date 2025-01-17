#include <memory/gdt.h>
#include <core/screen.h>

static struct GDTDescriptor gdt_descriptors[3];

void gdt_init(int32_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt_descriptors[index].base_low = base & 0xFFFF;
	gdt_descriptors[index].base_middle = (base >> 16) & 0xFF;
	gdt_descriptors[index].base_high = (base >> 24) & 0xFF;

	gdt_descriptors[index].limit_low = limit & 0xFFFF;
	gdt_descriptors[index].limit_flags = (limit >> 16) & 0xF;
	gdt_descriptors[index].limit_flags |= (flags << 4) & 0xF0;

	gdt_descriptors[index].access = access;
}

void segments_install_gdt()
{
	gdt_descriptors[0].base_low = 0;
	gdt_descriptors[0].base_middle = 0;
	gdt_descriptors[0].base_high = 0;
	gdt_descriptors[0].limit_low = 0;
	gdt_descriptors[0].access = 0;
	gdt_descriptors[0].limit_flags = 0;

	struct GDT* gdt = (struct GDT*) gdt_descriptors;
	gdt->address = (uint32_t) gdt_descriptors;
	gdt->size = (sizeof(struct GDTDescriptor) * 3) - 1;

	gdt_init(1, 0, 0xFFFFF, 0x9A, 0x0C);
	gdt_init(2, 0, 0xFFFFF, 0x92, 0x0C);

	segments_load_gdt(*gdt);
	segments_load_registers();
}

uint16_t gdt_code_segment_selector()
{
	return (uint8_t*) &gdt_descriptors[1] - (uint8_t*) gdt_descriptors;
}

uint16_t gdt_data_segment_selector()
{
	return (uint8_t*) &gdt_descriptors[2] - (uint8_t*) gdt_descriptors;
}
