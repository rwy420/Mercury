#include <exec/usermode.h>
#include <memory/common.h>
#include <memory/gdt.h>
#include <common/screen.h>

extern TSS tss;

void execute_user_mode(void* entry)
{
	memmove((void*) 0x500000, entry, 0x4000);
	uint32_t* user_stack = (uint32_t*) 0x600000;
	uint32_t* user_stack_ptr = (uint32_t*)(user_stack) + 4096;
	memset(user_stack, 0, 4096);

	tss.cs = 0x18;
	tss.ss = tss.ds = tss.es = tss.fs = tss.gs = 0x20;

	asm volatile(
		"cli;"
		"mov $0x20, %%ax;"
		"mov %%ax, %%ds;"
		"mov %%ax, %%es;"
		"mov %%ax, %%fs;"
		"mov %%ax, %%gs;"
		"mov %0, %%esp;"
		"push $0x20;"
		"push %0;"
		"pushf;"
		"push $0x18;"
		"push %1;"
		"iret;"
		:
		: "r" (((uint32_t) user_stack) + 4096), "r" ((void*) entry)
	);
}

static void kernel_mode()
{
	printf("\nBACK TO KERNEL MODE!!\n");
}

void kernel_switch_back()
{
	asm volatile(
		"cli;"
		"mov %0, %%esp;"
		"push $0x20;"
		"push %0;"
		"pushf;"
		"push $0x18;"
		"push %1;"
		"iret;"
		:
		: "r" (0x900000), "r" ((void*) kernel_mode)
	);
}
