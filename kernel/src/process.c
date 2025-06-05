#include <process.h>
#include <multitasking.h>
#include <common/screen.h>
#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/common.h>
#include <vesa.h>

extern Task* g_current_task;

void syscall_exit(CPUState* cpu)
{
	printf("Process exited with code ");
	print_uint32_t(cpu->ebx);
	printf("\n");
	kill_task(g_current_task->id);
	asm("int $32");
}

uint32_t create_user_process_pd(void* code)
{
	void* cr3 = kmalloc_aligned(4096, 4096 * 3);
	PageDirectory* directory = cr3; //malloc(4096 * 3);
	memset(directory, 0, sizeof(PageDirectory));
	for(uint32_t i = 0; i < 1024; i++) directory->entries[i] = 0x02;

	printf("Created USER CR3 at 0x");
	print_hex32((uint32_t) cr3);
	printf("\n");

	return (uint32_t) cr3;
}
