#ifndef __MERCURY__MULTITASKING_H
#define __MERCURY__MULTITASKING_H

#define MAX_TASKS 0xFF

#include <common/types.h>
#include <hardware/interrupts.h>

typedef struct Task
{
	uint8_t id;
	uint32_t esp, ebp, eip;
	uint32_t eax, ebx, ecx, edx, esi, edi;
	struct Task* next;
	struct Task* prev;
} Task;

void init_tasks();
Task* create_task(void(*entry)());
void schedule(CPUState* cpu);

void save_task();

#endif
