#include "common/types.h"
#include <multitasking.h>
#include <hardware/interrupts.h>
#include <memory/common.h>
#include <memory/mem_manager.h>
#include <common/screen.h>

Task tasks[MAX_TASKS];
Task* current_task;
Task* current_last;

void idle_task()
{
	while(1);
}

void init_tasks()
{
	memset(tasks, 0, sizeof(Task) * MAX_TASKS);

	for(int i = 0; i < MAX_TASKS; i++)
	{
		tasks[i].id = i;
	}

	Task* idle = &tasks[0];
	idle->eip = (uint32_t) idle_task;

	current_task = idle;
	current_last = idle;

	register_interrupt_handler(0x20, schedule);
}

void schedule()
{
	if(current_task->next == NULL_PTR)
	{
		current_task = &tasks[0];
	}
	else
	{
		current_task = current_task->next;
	}
}

Task* create_task(void(*entry)())
{
	for(int i = 0; i < MAX_TASKS; i++)
	{
		if(tasks[i].eip == 0)
		{
			Task* new = &tasks[i];
			new->eip = (uint32_t) entry;
			
			current_last->next = new;
			current_last = new;
			return new;
		}
	}

	return NULL_PTR;
}
