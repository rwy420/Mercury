#include <faults.h>
#include <common/screen.h>
#include <vesa.h>

char msg_gpf[] = "GP FAULT\n";

void handle_general_protection_fault(CPUState* cpu)
{	
	asm volatile(
		"mov $4, %%eax\n"
		"mov $1, %%ebx\n"
		"mov %0, %%ecx\n"
		"mov %1, %%edx\n"
		"int $0x80\n"
		:
		: "r"(msg_gpf), "r"(sizeof(msg_gpf))
		: "eax", "ebx", "ecx", "edx"
	);}

void handle_stack_fault(CPUState* cpu)
{
	printf_color("STACK PROTECTION FAULT\n", COLOR_RED, COLOR_BLACK);
}
