#include <common/types.h>
#include <memory/frames.h>
#include <multitasking.h>
#include <hardware/interrupts.h>
#include <hardware/pit.h>
#include <memory/common.h>
#include <memory/heap.h>
#include <memory/gdt.h>
#include <memory/paging.h>
#include <common/screen.h>
#include <exec/elf/elf_loader.h>
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

void tasks_init()
{
	task_list = NULL_PTR;
	g_current_task = NULL_PTR;

	create_task(dummy_task, true);
	create_task(idle_task, true);
	//create_task(idle_task2, true);
	//create_task(idle_task3, false);
}

Task* create_task(void entry(), int kernel)
{
	if(next_id > MAX_TASKS) return NULL_PTR;

	Task* task = (Task*) kmalloc(sizeof(Task));
	if(!task) return NULL_PTR;
	memset(task, 0, sizeof(Task));	
	
	void* stack = alloc_frames(4);
	for(uint32_t i = (uint32_t) stack; i < (uint32_t) stack + 4 * FRAME_SIZE; i += FRAME_SIZE) map_page((void*) i, (void*) i);
	memset(stack, 0x0, 0x4000);
	CPUState* cpu_state = (CPUState*) (((uint32_t) stack) + 0x4000 - sizeof(CPUState));
	
	if(!kernel)
	{
		PageDirectory* pd = create_kernel_pd();
		task->cr3 = virtual_to_physical(pd);
	}
	else
	{
		task->cr3 = (uint32_t) g_kernel_pd;
	}

	for(int i = 0; i < 4; i++)
	{
		void* address = (void*) ((uint32_t) stack) + i * FRAME_SIZE;
		map_page(address, address);
		if(!kernel) map_page_pd((PageDirectory*) task->cr3, address, address);
	}

	task->kernel = kernel;
	task->esp = (uint32_t) cpu_state;

	cpu_state->ebp = task->esp;

	cpu_state->eip = (uint32_t) entry;
	cpu_state->cs = 0x08;
	cpu_state->eflags = 0x202;
	cpu_state->esp = ((uint32_t) cpu_state);
	cpu_state->ss = 0x10;
		

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

uint32_t schedule(uint32_t esp) 
{
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
			__asm__ __volatile__("movl %%EAX, %%CR3" : : "a" (g_current_task->cr3));
			return (uint32_t) g_current_task->esp;
		}

	} while(g_current_task != start);

	return esp;
}
