#ifndef __MERCURY__MULTITASKING_H
#define __MERCURY__MULTITASKING_H

#define MAX_TASKS 0xFF

#include <common/types.h>

typedef struct Task
{
	uint8_t id;
	uint32_t eip, esp;
	struct Task* next;
	struct Task* prev;
} Task;

void init_tasks();
void schedule();
Task* create_task(void(*entry)());

#endif
