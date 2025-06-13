#ifndef __MERCURY__MULTITASKING_H
#define __MERCURY__MULTITASKING_H

#define MAX_TASKS 0xFF

#include <common/types.h>
#include <hardware/interrupts.h>

typedef enum
{
	TASK_RUNNING,
	TASK_WAITING,
	TASK_READY,
	TASK_TERMINATED
} TASK_STATE;

typedef struct Task
{
	uint8_t id;
	uint32_t esp, ebp, eip;
	uint32_t eax, ebx, ecx, edx, esi, edi;
	uint32_t cr3;
	uint32_t flags;
	TASK_STATE state;
	struct Task* next;
} __attribute__((packed)) Task;

void tasks_init();
Task* create_task(void(*entry)());
void kill_task(uint8_t id);
void schedule(CPUState* cpu);

void restore_and_switch();

#endif
