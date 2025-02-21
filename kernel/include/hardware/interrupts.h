#ifndef __MERCURY__HARDWARE__INTERRUPTS_H
#define __MERCURY__HARDWARE__INTERRUPTS_H

#include <hardware/pic.h>
#include <common/types.h>
#include <memory/gdt.h>
#include <hardware/port.h>

typedef struct
{
	uint16_t size;
	uint32_t address;
} __attribute__((packed)) IDT;

typedef struct
{
	uint16_t offset_low;
	uint16_t segment_selector;

	uint8_t reserved;
	uint8_t type_attribute;
	uint16_t offset_high;
} __attribute__((packed)) IDTDescriptor;

void install_idt();

void handle_irq_00();
void handle_irq_01();
void handle_irq_02();
void handle_irq_03();
void handle_irq_04();
void handle_irq_05();
void handle_irq_06();
void handle_irq_07();
void handle_irq_08();
void handle_irq_09();
void handle_irq_10();
void handle_irq_11();
void handle_irq_12();
void handle_irq_13();
void handle_irq_14();
void handle_irq_15();

void handle_irq_32();
void handle_irq_33();
void handle_irq_49();
void handle_irq_128();

typedef struct
{
	uint32_t eax;
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t error;
} __attribute__((packed)) CPUState;

typedef void (*isr_t)(CPUState* cpu);

void register_interrupt_handler(uint8_t n, isr_t handler);
int interrupt_handler(CPUState cpu_state, uint32_t interrupt);

#endif 
