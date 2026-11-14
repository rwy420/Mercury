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

int load_process(char* path)
{
	// Load file
	uint32_t file = vfs_open(path);
	uint32_t file_size = vfs_get_size(file);
	char* buffer = kmalloc(file_size);
	vfs_read(file, buffer, file_size);

	// Create user PD
	PageDirectory* pd = (PageDirectory*) virtual_to_physical(create_kernel_pd());

	void (*entry0)() = elf32_load(buffer, file_size, pd);
	create_task(entry0, 0, 0x1000, pd);
	
	return 0;
}
