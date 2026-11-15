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

	dbg_printf("Page fault at address: 0x");
	dbg_print_hex32(address);
	dbg_printf(" error: ");
	dbg_print_hex32(cpu->error_code);
	dbg_printf("\n");

	if(!(cpu->error_code & 0x1)) dbg_printf(" - Not present\n");
	if(cpu->error_code & 0x2) dbg_printf(" - Write\n");
	if(cpu->error_code & 0x4) dbg_printf(" - User\n");
	if(cpu->error_code & 0x8) dbg_printf(" - Reserved bit voilation\n");
	if(cpu->error_code & 0x10) dbg_printf(" - Instruction fetched\n");

	terminate_or_panic("PAGE FAULT", cpu);
}

void terminate_or_panic(char* reason, CPUState* cpu)
{
	if(!(g_current_task->flags & TASK_KERNEL))
	{
		g_current_task->state = TASK_TERMINATED;
		kprintf("%s (Task terminated)\n", reason);
		asm("int $0x20"); // yield to stop the execution of the terminated task right here
	}

	panic(reason, cpu);
}

