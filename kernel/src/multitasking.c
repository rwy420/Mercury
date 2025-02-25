#include "exec/usermode.h"
#include <multitasking.h>
#include <shell.h>
#include <hardware/interrupts.h>
#include <hardware/pit.h>
#include <memory/common.h>
#include <memory/mem_manager.h>
#include <memory/gdt.h>
#include <common/screen.h>

Task tasks[MAX_TASKS];
Task* g_current_task;
Task* current_last;

extern TSS g_tss;

void kernel_schedule();

void dummy_task()
{
	asm("int $32");
}

void idle_task()
{
	printf("PART 1\n");
	asm("int $32");
	printf("PART 2\n");
	while(1);
}

void init_tasks()
{
	memset(tasks, 0, sizeof(Task) * MAX_TASKS);

	for(int i = 0; i < MAX_TASKS; i++)
	{
		tasks[i].id = i;
	}

	Task* dummy = &tasks[0];
	dummy->eip = (uint32_t) dummy_task;

	Task* idle = &tasks[1];
	idle->eip = (uint32_t) idle_task;
	idle->esp = 0x800000;
	idle->next = idle;

	dummy->next = idle;

	g_current_task = dummy;
	current_last = idle;
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

void schedule(CPUState* cpu) 
{
	g_current_task->esp = cpu->esp;
	g_current_task->eip = cpu->eip;
	g_current_task->eax = cpu->eax;
	g_current_task->ebp = cpu->ebp;
	g_current_task->ecx = cpu->ecx;
	g_current_task->edi = cpu->edi;
	g_current_task->edx = cpu->edx;
	g_current_task->esi = cpu->esi;
	g_current_task->ebx = cpu->ebx;

	if(g_current_task->next == NULL_PTR)
	{
		g_current_task = &tasks[1];
	}
	else
	{
		g_current_task = g_current_task->next;
	}

	printf(""); // ????

	asm volatile(
		"mov $0x20, %%ax;"
		"mov %%ax, %%ds;"
		"mov %%ax, %%es;"
		"mov %%ax, %%fs;"
		"mov %%ax, %%gs;"	
		"mov %0, %%esp;"
		"push $0x20;"
		"push %%esp;"
		"push $0x200200;"
		"push $0x18;"
		"push %1;"
		"iret;"
		:
		: "r" ((g_current_task->esp) + 4096), "r" (g_current_task->eip)
	);
}
