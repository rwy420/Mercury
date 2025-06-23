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
	uint32_t esp, eip;
	uint32_t ebp, eax, ebx, ecx, edx;
	uint32_t esi, edi;
	uint32_t cr3;
	uint32_t flags;
	int kernel;
	TASK_STATE state;
	struct Task* next;
} __attribute__((packed)) Task;

void tasks_init();
Task* create_task(void(*entry)(), int kernel);
void kill_task(uint8_t id);
void schedule(CPUState* cpu);

void restore_and_switch();

#endif
