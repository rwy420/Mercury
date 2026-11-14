#include <multitasking.h>
#include <thread.h>

extern PageDirectory* g_kernel_pd;

int kthread_create(char* name, void (*entry)(void))
{
	Task* task = create_task(entry, TASK_KERNEL, 0x1000, g_kernel_pd);
	
	return 0;
}
