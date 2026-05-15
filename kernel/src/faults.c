#include <faults.h>
#include <common/screen.h>
#include <vesa.h>

void handle_general_protection_fault(CPUState* cpu)
{
	printf_color("GENERAL PROTECTION FAULT\n", COLOR_RED, COLOR_BLACK);
}

void handle_stack_fault(CPUState* cpu)
{
	printf_color("STACK PROTECTION FAULT\n", COLOR_RED, COLOR_BLACK);
}
