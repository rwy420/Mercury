#include "vm.h"
#include <process.h>
#include <multitasking.h>
#include <common/screen.h>
#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/common.h>
#include <vesa.h>
#include <exec/elf32/elf32loader.h>

extern Task* g_current_task;

void syscall_exit(CPUState* cpu)
{
	printf("Process exited with code ");
	print_uint32_t(cpu->ebx);
	printf("\n");
	kill_task(g_current_task->id);
	asm("int $32");
}

int process_create(char* path, int argc, char** argv)
{
	// Load file
	uint32_t file = vfs_open(path);
	uint32_t file_size = vfs_get_size(file);
	char* buffer = kmalloc(file_size);
	vfs_read(file, buffer, file_size);

	// Create user PD
	PageDirectory* pd = (PageDirectory*) virtual_to_physical(create_kernel_pd());

	void (*entry)() = elf32_load(buffer, file_size, pd);
	Task* task = create_task(entry, 0, 0x1000, pd);

	// Set up entry frame
	char* task_stack_8 = (char*) task->esp;
	uint32_t* task_stack_32 = (uint32_t*) task->esp;
	task_stack_32[0] = argc;
	uint32_t stack_idx = sizeof(uint32_t*) + argc * sizeof(void*); // The amount of bytes we need to skip before putting the argv data

	for(int i = 0; i < argc; i++)
	{
		task_stack_32[1 + i] = (uint32_t) &task_stack_8[stack_idx];

		for(int c = 0; argv[i][c] != '\0'; c++)
		{
			task_stack_8[stack_idx++] = argv[i][c];
		}

		stack_idx++;
		task_stack_8[stack_idx] = '\0';
	}

	return 0;
}
