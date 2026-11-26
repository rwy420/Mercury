#include <panic.h>
#include <vm.h>
#include <faults.h>
#include <vesa.h>
#include <multitasking.h>
#include <printf.h>

extern Task* g_current_task;

void handle_invalid_opcode(CPUState* cpu)
{
	terminate_or_panic("INVALID OPCODE", cpu);
}

void handle_general_protection_fault(CPUState* cpu)
{
	terminate_or_panic("GENERAL PROTECTION FAULT", cpu);
}

void handle_page_fault(CPUState* cpu)
{
	uint32_t address = 0;
	__asm__ __volatile__("movl %%CR2, %0" : "=r" (address));

	dbgprintf("Page fault at address 0x%x error: 0x%x\n", address, cpu->error_code);

	if(!(cpu->error_code & 0x1)) dbgprintf(" - Not present\n");
	if(cpu->error_code & 0x2) dbgprintf(" - Write\n");
	if(cpu->error_code & 0x4) dbgprintf(" - User\n");
	if(cpu->error_code & 0x8) dbgprintf(" - Reserved bit voilation\n");
	if(cpu->error_code & 0x10) dbgprintf(" - Instruction fetched\n");

	terminate_or_panic("PAGE FAULT", cpu);
}

void terminate_or_panic(const char* reason, CPUState* cpu)
{
	if(!(g_current_task->flags & TASK_KERNEL))
	{
		g_current_task->state = TASK_TERMINATED;
		kprintf("%s (Task terminated)\n", reason);
		asm("int $0x20"); // yield to stop the execution of the terminated task right here
	}

	panic(reason, cpu);
}

