#include <memory/paging.h>
#include <exec/usermode.h>
#include <memory/common.h>
#include <memory/gdt.h>
#include <common/screen.h>

extern TSS g_tss;

void execute_user_mode(void* entry)
{
	for(int i = 0; i < 0x4000; i += PAGE_SIZE)
	{
		map_page((void*) 0x500000 + i, (void*) 0x500000 + i, PTE_PRESENT | PTE_USER | PTE_RW);
	}

	memcpy((void*) 0x500000, entry, 0x4000);

	map_page((void*) 0x5ff000, (void*) 0x5ff000, PTE_PRESENT | PTE_USER | PTE_RW);
	map_page((void*) 0x600000, (void*) 0x600000, PTE_PRESENT | PTE_USER | PTE_RW);

	uint32_t* user_stack = (uint32_t*) 0x600000;
	uint32_t* user_stack_ptr = (uint32_t*)(user_stack) + 4096;
	memset(user_stack, 0, 4096);

	g_tss.cs = 0x18;
	g_tss.ss = g_tss.ds = g_tss.es = g_tss.fs = g_tss.gs = 0x20;

	asm volatile(
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

	while(1);
}

void kernel_switch_back()
{
	asm volatile(
		"sti;"
		"mov $0x10, %%ax;"
		"mov %%ax, %%ds;"
		"mov %%ax, %%es;"
		"mov %%ax, %%fs;"
		"mov %%ax, %%gs;"
		"mov %0, %%esp;"
		"push $0x10;"
		"push %0;"
		"pushf;"
		"push $0x08;"
		"push %1;"
		"iret;"
		:
		: "r" (0x90000), "r" ((void*) kernel_mode)
	);
}

void syscall_exit(int status)
{
	printf("Process exited with code");
	print_hex32(status);
	printf("\n");
	kernel_switch_back();
}
