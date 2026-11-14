// TODO: Add a reaper task to remove TERMINATED tasks and free their memory

#include "memory/paging.h"
#include <common/types.h>
#include <memory/frames.h>
#include <multitasking.h>
#include <hardware/interrupts.h>
#include <hardware/pit.h>
#include <memory/common.h>
#include <memory/heap.h>
#include <memory/gdt.h>
#include <common/screen.h>
#include <exec/elf32/elf32loader.h>
#include <vesa.h>

Task* task_list = NULL_PTR;
Task* g_current_task = NULL_PTR;
uint32_t next_id = 1;

extern TSS g_tss;
extern PageDirectory* g_kernel_pd;

void dummy_task()
{
	asm("int $32");
}

const char msg[] = "printf via write syscall!\n";

void test_task()
{
    uint32_t len = sizeof(msg) - 1;

	asm volatile(
		"mov $4, %%eax\n"
		"mov $1, %%ebx\n"
		"mov %0, %%ecx\n"
		"mov %1, %%edx\n"
		"int $0x80\n"
		:
		: "r"(msg), "r"(len)
		: "eax", "ebx", "ecx", "edx"
	);

    asm volatile(
        "mov $1, %%eax \n"
        "mov $1, %%ebx \n"
        "int $0x80"
        :
        :
        : "eax", "edi"
    );
}

void idle_task()
{
	uint32_t counter = 100;
	while(1)
	{
		printf("Task 1 ");
		print_hex32(counter++);
		printf("\n");
		for(volatile uint32_t i = 0; i < 1000000; i++) for(volatile uint32_t i = 0; i < 1000; i++);
	}
}

void idle_task2()
{
	uint32_t counter = 100;
	while(1)
	{
		printf("Task 2 ");
		print_hex32(counter++);
		printf("\n");
		for(volatile uint32_t i = 0; i < 1000000; i++) for(volatile uint32_t i = 0; i < 1000; i++);
	}
}

void tasks_init()
{
	task_list = NULL_PTR;
	g_current_task = NULL_PTR;

	create_task(dummy_task, TASK_KERNEL, 0, 0);
	create_task(idle_task, TASK_KERNEL, 0, 0);
}

Task* create_task(void entry(), uint32_t flags, uint32_t stack_size, PageDirectory* pd)
{
	if(next_id > MAX_TASKS) return NULL_PTR;
	if(stack_size == 0) stack_size = PAGE_SIZE; // Task stack size will default to smallest size, which is the size of one page

	Task* task = (Task*) kmalloc(sizeof(Task));
	if(!task) return NULL_PTR;
	memset(task, 0, sizeof(Task));	

	task->flags = flags;

	// Tasks that don't belong to the kernel will get their own page directory to isolate them from other non-kernel tasks
	if(pd == 0 && flags & TASK_KERNEL)
	{
		task->cr3 = (uint32_t) g_kernel_pd;
	}
	else if(pd == 0 && !(flags & TASK_KERNEL))
	{
		printf("<Mercury> Can't create non-kernel task with zero PD\n");
		return 0;
	}
	else
	{
		task->cr3 = (uint32_t) pd;
	}

	stack_size = ALIGN_UP(stack_size, FRAME_SIZE);
	void* stack = alloc_frames(stack_size / FRAME_SIZE);
	
	for(uint32_t i = (uint32_t) stack; i < (uint32_t) stack + stack_size; i += PAGE_SIZE)
	{
		map_page_pd_flags(g_kernel_pd, (void*) i, (void*) i, PDE_PRESENT | PDE_RW | PDE_USER);
		map_page_pd_flags((PageDirectory*) task->cr3, (void*) i, (void*) i, PDE_PRESENT | PDE_RW | PDE_USER);
	}

	memset(stack, 0x0, stack_size);
	CPUState* cpu_state = (CPUState*) (((uint32_t) stack) + stack_size - sizeof(CPUState)); // Where the task state will be saved
																							//
	task->esp = (uint32_t) cpu_state;
	uint32_t* argc = (uint32_t*) (task->esp);
	print_hex32((uint32_t )argc);
	*argc = 0x1;
	printf("\n");

	cpu_state->ebp = task->esp;

	cpu_state->eip = (uint32_t) entry;

	// Flags and segments
	if(flags & TASK_KERNEL)
	{
		cpu_state->cs = 0x08;
		cpu_state->eflags = 0x202;
		cpu_state->esp = ((uint32_t) cpu_state);
		cpu_state->ss = 0x10;
	}
	else
	{
		cpu_state->cs = 0x1B;
		cpu_state->eflags = 0x202;
		cpu_state->esp = ((uint32_t) cpu_state);
		cpu_state->ss = 0x23;

		// The kernel will use a different stack when called from a non-kernel task
		void* kernel_stack = alloc_frames(4);
		for(uint32_t i = (uint32_t) kernel_stack; i < (uint32_t) kernel_stack + 4 * FRAME_SIZE; i += FRAME_SIZE) 
		{
			map_page_pd_flags((PageDirectory*) task->cr3, (void*) i, (void*) i, PDE_PRESENT | PDE_RW);
			map_page_pd_flags(g_kernel_pd, (void*) i, (void*) i, PDE_PRESENT | PDE_RW);
		}

		task->kernel_esp = (uint32_t) kernel_stack + 0x4000;
	}

	task->id = next_id++;
	task->state = TASK_READY;
	task->next = NULL_PTR;

	if(task_list == NULL_PTR)
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

// TODO: fix the bug where the first task created won't execute; this is probably because the first time schedule is ran, it likely sets the first 
// task's state to what the kernel itself was at
uint32_t schedule(uint32_t esp) 
{
	// The CPUState was pushed onto the stack, right above the address in esp
	CPUState* cpu = (CPUState*) esp;
	set_pd(g_kernel_pd);

	pic_confirm(0x20);

	if(g_current_task == NULL_PTR)
	{
		g_current_task = task_list;
		return esp;
	}

	Task* start = g_current_task;

	do
	{
		g_current_task = g_current_task->next ? g_current_task->next : task_list;

		if(g_current_task->state == TASK_READY || g_current_task->state == TASK_RUNNING)
		{ 
			if(g_current_task->kernel_esp != 0x00)
			{
				asm volatile("mov $0x23, %AX");
				asm volatile("mov %AX, %DS");
				asm volatile("mov %AX, %ES");
				asm volatile("mov %AX, %FS");
				asm volatile("mov %AX, %GS");
			}

			g_tss.esp0 = g_current_task->kernel_esp;
			__asm__ __volatile__("movl %%EAX, %%CR3" : : "a" (g_current_task->cr3));
			return (uint32_t) g_current_task->esp;
		}

	} while(g_current_task != start);

	return esp;
}
