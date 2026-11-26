#include <hardware/pit.h>
#include <common/types.h>
#include <memory/gdt.h>
#include <memory/common.h>
#include <hardware/interrupts.h>
#include <hardware/pic.h>
#include <syscall.h>
#include <multitasking.h>

extern Task* g_current_task;

isr_t interrupt_handers[256];

uint16_t hardware_interrupt_offset = 0x20;

void register_interrupt_handler(uint8_t n, isr_t handler)
{
	interrupt_handers[n] = handler;
}

IDTDescriptor idt_descriptors[256] = {0x00};
IDT idt;

void interrupts_init_descriptor(uint32_t index, uint32_t address, uint32_t attribute)
{
	idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF;
	idt_descriptors[index].offset_low = (address & 0xFFFF);

	idt_descriptors[index].segment_selector = 0x08;
	idt_descriptors[index].reserved = 0x00;

	idt_descriptors[index].type_attribute = attribute;
}

void install_idt()
{
	memset(interrupt_handers, 0, sizeof(interrupt_handers));

	interrupts_init_descriptor(0x06, (uint32_t) handle_irq_06, 0xEE);
	interrupts_init_descriptor(0x0D, (uint32_t) handle_irq_13, 0xEE);
	interrupts_init_descriptor(0x0E, (uint32_t) handle_irq_14, 0xEE);

	interrupts_init_descriptor(0x20, (uint32_t) handle_irq_32, 0xEE);
	interrupts_init_descriptor(0x21, (uint32_t) handle_irq_33, 0xEE);
	interrupts_init_descriptor(0x80, (uint32_t) handle_irq_128, 0xEF/* Trap gate*/);

	pic_remap(0x20, 0x28);

	idt.address = (int32_t) &idt_descriptors;
	idt.size = sizeof(IDTDescriptor) * 256;
	asm volatile("lidt %0; sti;" : : "m" (idt));	
}

uint32_t interrupt_handler(uint32_t interrupt, uint32_t esp)
{
	CPUState* cpu_state = (CPUState*) esp;		

	g_current_task->kernel_esp = esp;

	if(interrupt == 0x20) 
	{
		esp = pit_handle_interrupt(esp);
	}

	if(interrupt_handers[interrupt] != 0x00)
	{
		interrupt_handers[interrupt](cpu_state);
	}

	if(hardware_interrupt_offset <= interrupt && interrupt < hardware_interrupt_offset + 16)
	{
		pic_confirm(interrupt);
	}

	//if(cpu_state->ss == 0x23)
	//{
		if(!(g_current_task->flags & TASK_KERNEL))
		{
		    asm volatile(
				"mov $0x23, %%ax\n"
				"mov %%ax, %%ds\n"
				"mov %%ax, %%es\n"
				"mov %%ax, %%fs\n"
				"mov %%ax, %%gs\n"
				::: "ax"
			);
		}
	//}

	return esp;
};
