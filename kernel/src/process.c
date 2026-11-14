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
	uint32_t* task_stack = (uint32_t*) task->esp;
	task_stack[0] = argc;

	for(int i = 1; i < argc; i++) task_stack[i] = (uint32_t) argv[i - 1];
	
	return 0;
}
