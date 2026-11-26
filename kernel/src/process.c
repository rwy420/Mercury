#include <process.h>
#include <multitasking.h>
#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/common.h>
#include <printf.h>
#include <exec/elf32/elf32loader.h>
#include <fs/vfs/vfs.h>
#include <vm.h>

extern Task* g_current_task;

void syscall_exit(CPUState* cpu)
{
	kprintf("Process exited with code %u\n", (uint32_t) cpu->ebx);
	g_current_task->state = TASK_TERMINATED;
	asm("int $32");
}

int syscall_fork(CPUState* cpu)
{
	dbgprintf("UNHANDLED SYSCALL: fork\n");
	return 0;
}

int syscall_execve(CPUState* cpu)
{
	dbgprintf("UNHANDLED SYSCALL: execve\n");
	return 0;
}

int syscall_wait4(CPUState* cpu)
{
	dbgprintf("UNHANDLED SYSCALL: wait4\n");
	return 0;
} 

int process_create(const char* path, int argc, char** argv)
{
	uint32_t file = vfs_open(path);
	uint32_t file_size = vfs_get_size(file);
	char* buffer = kmalloc(file_size);
	vfs_read(file, buffer, file_size);

	PageDirectory* pd = (PageDirectory*) virtual_to_physical(create_kernel_pd());
	
	void (*entry)() = elf32_load(buffer, file_size, pd, 0)->entry;
	Task* task = create_task(entry, 0, 0x1000, pd);

	// TODO: envp passing
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

	kfree(buffer);

	return 0;
}
