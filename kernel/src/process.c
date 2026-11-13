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

int load_process(char* path, uint32_t size)
{
	// Load file
	uint32_t test = vfs_open(path);
	char* buffer = kmalloc(size);
	vfs_read(test, buffer, size);

	// Create user PD
	PageDirectory* pd = create_kernel_pd();

	void (*entry0)() = elf32_load(buffer, 13116, pd);
	//create_task(entry0, 0, 0x1000);
	
	return 0;
}
