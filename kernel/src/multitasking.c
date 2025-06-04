#include "exec/elf/elf_loader.h"
#include "fs/fat16/fat16.h"
#include "process.h"
#include <multitasking.h>
#include <shell.h>
#include <hardware/interrupts.h>
#include <hardware/pit.h>
#include <memory/common.h>
#include <memory/mem_manager.h>
#include <memory/gdt.h>
#include <memory/paging.h>
#include <common/screen.h>

Task* task_list = NULL_PTR;
Task* g_current_task = NULL_PTR;
uint32_t next_id = 1;

extern TSS g_tss;

// Messages need to have a fixed memory address
char* msg1 = "Task: ";
char* msg3 = "Task2: ";
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

void idle_task2()
{
	static uint32_t counter = 0;
	while(1)
	{
		printf(msg3);
		print_hex32(counter++);
		printf(msg2);
		for(volatile uint32_t i = 0; i < 1000000; i++) for(volatile uint32_t i = 0; i < 1000; i++);
	}
}

void tasks_init()
{
	//Fix this ..
	create_task((void*) 0x0, 0x0);

	create_task(idle_task2, 0x700000);
	create_task(idle_task, 0x800000);

	int fd = fat16_open("/BIN/APP.ELF", 'r');
	int size = fat16_size("/BIN/APP.ELF");
	char* buffer = kmalloc(size);
	fat16_read(fd, buffer, size);
	void(*entry)();
	entry = image_load(buffer, size, 0);
	create_task(entry, 0x900000);
}

Task* create_task(void(*entry)(), uint32_t esp)
{
	if(next_id > MAX_TASKS) return NULL_PTR;

	Task* task = (Task*) kmalloc(sizeof(Task));
	if(!task) return NULL_PTR;

	/*void* stack = kmalloc(4096);

	memset(stack, 0, 4096);
	uint32_t esp = (uint32_t) stack;*/

	//task->esp = 0xF00000;
	task->eip = (uint32_t) entry;
	//task->eip = 0x10000;
	task->esp = esp;
	//task->cr3 = create_user_process_pd(entry);

	task->id = next_id++;
	task->state = TASK_READY;
	task->next = NULL_PTR;

	if(!task_list)
	{
		task_list = task;
	}
	else
	{
		Task* t = task_list;
		while(t->next) t = t->next;
		t->next = task;
	}

	return task;
}

void kill_task(uint8_t id)
{
	Task* prev = NULL_PTR, *current = task_list;

	while(current)
	{
		if(current->id == id)
		{
			if(prev) prev->next = current->next;
			else task_list = current->next;

			kfree((void*) current->esp);
			kfree(current);	
		}

		prev = current;
		current = current->next;
	}
}

void schedule(CPUState* cpu) 
{
	pic_confirm(0x20);

	if(!g_current_task)
	{
		g_current_task = task_list;
		return;
	}

	g_current_task->eip = cpu->eip;
	g_current_task->esp = cpu->esp;
	g_current_task->ebp = cpu->ebp;

	g_current_task->eax = cpu->eax;
	g_current_task->ebx = cpu->ebx;
	g_current_task->ecx = cpu->ecx;
	g_current_task->edx = cpu->edx;
	g_current_task->esi = cpu->esi;
	g_current_task->edi = cpu->edi;

	Task* start = g_current_task;

	do
	{
		g_current_task = g_current_task->next ? g_current_task->next : task_list;

		if(g_current_task->state == TASK_READY || g_current_task->state == TASK_RUNNING)
		{
			g_tss.cs = 0x18;
			g_tss.ss = g_tss.ds = g_tss.es = g_tss.fs = g_tss.gs = 0x20;
			restore_and_switch();

			break;
		}

	} while(g_current_task != start);
}
