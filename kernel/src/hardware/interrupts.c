#include <common/types.h>
#include <memory/gdt.h>
#include <memory/common.h>
#include <hardware/interrupts.h>
#include <hardware/pic.h>
#include <common/screen.h>
#include <syscalls.h>

#define MASTER_COMMAND_PORT 0x20
#define SLAVE_COMMAND_PORT 0xA0
#define MASTER_DATA_PORT 0x21
#define SLAVE_DATA_PORT 0xA1

isr_t interrupt_handers[256];

uint16_t hardware_interrupt_offset;

void register_interrupt_handler(uint8_t n, isr_t handler)
{
	interrupt_handers[n] = handler;
}

IDTDescriptor idt_descriptors[256] = {0x00};
IDT idt;

void interrupts_init_descriptor(int32_t index, uint32_t address)
{
	idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF;
	idt_descriptors[index].offset_low = (address & 0xFFFF);

	idt_descriptors[index].segment_selector = 0x08;
	idt_descriptors[index].reserved = 0x00;

	idt_descriptors[index].type_attribute = (0x01 << 7) | (0x00 << 6) | (0x00 << 5) | 0xE;
}

void install_idt()
{
	memset(interrupt_handers, 0, sizeof(interrupt_handers));

	interrupts_init_descriptor(0x00, (uint32_t) handle_irq_00);
	interrupts_init_descriptor(0x01, (uint32_t) handle_irq_01);
	interrupts_init_descriptor(0x02, (uint32_t) handle_irq_02);
	interrupts_init_descriptor(0x03, (uint32_t) handle_irq_03);
	interrupts_init_descriptor(0x04, (uint32_t) handle_irq_04);
	interrupts_init_descriptor(0x05, (uint32_t) handle_irq_05);
	interrupts_init_descriptor(0x06, (uint32_t) handle_irq_06);
	interrupts_init_descriptor(0x07, (uint32_t) handle_irq_07);
	interrupts_init_descriptor(0x08, (uint32_t) handle_irq_08);
	interrupts_init_descriptor(0x09, (uint32_t) handle_irq_09);
	interrupts_init_descriptor(0x0A, (uint32_t) handle_irq_10);
	interrupts_init_descriptor(0x0B, (uint32_t) handle_irq_11);
	interrupts_init_descriptor(0x0C, (uint32_t) handle_irq_12);
	interrupts_init_descriptor(0x0D, (uint32_t) handle_irq_13);
	interrupts_init_descriptor(0x0E, (uint32_t) handle_irq_14);
	interrupts_init_descriptor(0x0F, (uint32_t) handle_irq_15);
	interrupts_init_descriptor(0x21, (uint32_t) handle_irq_33);
	interrupts_init_descriptor(0x31, (uint32_t) handle_irq_49);
	interrupts_init_descriptor(0x80, (uint32_t) handle_irq_128);

	idt.address = (int32_t) &idt_descriptors;
	idt.size = sizeof(IDTDescriptor) * 256;
	asm volatile("lidt %0" : : "m" (idt));

	pic_remap(0x20, 0x28);
}

void interrupt_handler(CPUState cpu_state, uint32_t interrupt)
{
	if(interrupt_handers[interrupt] != NULL_PTR)
	{
		interrupt_handers[interrupt](&cpu_state);
		pic_confirm(interrupt);
	}

	if(hardware_interrupt_offset <= interrupt && interrupt < hardware_interrupt_offset + 16)
	{
		outb_slow(MASTER_COMMAND_PORT, 0x20);

		if(hardware_interrupt_offset + 8 <= interrupt)
			outb_slow(SLAVE_COMMAND_PORT, 0x20);
	}
}
