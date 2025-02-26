#include "exec/elf/elf_loader.h"
#include "exec/usermode.h"
#include "fs/fat16/fat16.h"
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
char* msg1 = "Task: ";
char* msg2 = "\n";

void kernel_schedule();

void dummy_task()
{
	asm("int $32");
}

void idle_task()
{
	static uint32_t counter = 0;
	while(1)
	{
		printf(msg1);
		print_hex32(counter++);
		printf(msg2);
		for(volatile uint32_t i = 0; i < 1000000; i++) for(volatile uint32_t i = 0; i < 1000; i++);
	}
}

void init_tasks()
{
	memset(tasks, 0, sizeof(Task) * MAX_TASKS);

	for(int i = 0; i < MAX_TASKS; i++)
	{
		tasks[i].id = i;
	}

	Task* dummy = &tasks[0];
	dummy->flags = TASK_OK;
	dummy->eip = (uint32_t) dummy_task;

	Task* idle = &tasks[1];
	idle->flags = TASK_OK;
	idle->eip = (uint32_t) idle_task;
	idle->esp = 0x800000;	

	int fd = fat16_open("/BIN/APP.ELF", 'r');
	int size = fat16_size("/BIN/APP.ELF");
	char* buffer = malloc(size);
	fat16_read(fd, buffer, size);
	void(*entry)();
	entry = image_load(buffer, size, 0);

	Task* test = &tasks[2];
	test->flags = TASK_OK;
	test->eip = (uint32_t) entry;
	test->esp = 0x700000;
	idle->next = test;
	dummy->next = idle;

	g_current_task = dummy;
	current_last = test;
}

Task* create_task(void(*entry)(), uint32_t esp)
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

void kill_task(uint8_t id)
{
	Task* task = &tasks[id];
	task->flags = TASK_TERMINATED;
}

void schedule(CPUState* cpu) 
{
	pic_confirm(0x20);

	g_current_task->eip = cpu->eip;
	g_current_task->esp = cpu->esp;
	g_current_task->ebp = cpu->ebp;

	g_current_task->eax = cpu->eax;
	g_current_task->ebx = cpu->ebx;
	g_current_task->ecx = cpu->ecx;
	g_current_task->edx = cpu->edx;
	g_current_task->esi = cpu->esi;
	g_current_task->edi = cpu->edi;

	if(g_current_task->id == current_last->id)
	{
		g_current_task = &tasks[1];
	}
	else
	{
		g_current_task = g_current_task->next;
	}

	g_tss.cs = 0x18;
	g_tss.ss = g_tss.ds = g_tss.es = g_tss.fs = g_tss.gs = 0x20;

	restore_and_switch();
}
