#ifndef __MERCURY__MULTITASKING_H
#define __MERCURY__MULTITASKING_H

#define MAX_TASKS 0xFF

#include <common/types.h>
#include <hardware/interrupts.h>

typedef enum
{
	TASK_OK = 0x01,
	TASK_TERMINATED = 0x02
} TASK_FLAGS;

typedef struct Task
{
	uint8_t id;
	uint32_t esp, ebp, eip;
	uint32_t eax, ebx, ecx, edx, esi, edi;
	uint32_t flags;
	struct Task* next;
	struct Task* prev;
} __attribute__((packed)) Task;

void init_tasks();
Task* create_task(void(*entry)(), uint32_t esp);
void kill_task(uint8_t id);
void schedule(CPUState* cpu);

void restore_and_switch();

#endif
