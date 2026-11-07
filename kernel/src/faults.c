#include "vm.h"
#include <faults.h>
#include <common/screen.h>
#include <vesa.h>

char c[] = "GENRAL PROTECTION FAUL";

void handle_general_protection_fault(CPUState* cpu)
{
	printf("#GF\n");
}

void handle_stack_fault(CPUState* cpu)
{
	printf_color("STACK PROTECTION FAULT\n", COLOR_RED, COLOR_BLACK);
}

void handle_page_fault(CPUState* cpu)
{
	uint32_t address = 0;
	__asm__ __volatile__("movl %%CR2, %0" : "=r" (address));

	printf("Page fault at address: 0x");
	print_hex32(address);
	printf(" error: ");
	print_hex32(cpu->error_code);
	printf("\n");

	if(!(cpu->error_code & 0x1)) printf(" - Not present\n");
	if(cpu->error_code & 0x2) printf(" - Write\n");
	if(cpu->error_code & 0x4) printf(" - User\n");
	if(cpu->error_code & 0x8) printf(" - Reserved bit voilation\n");
	if(cpu->error_code & 0x10) printf(" - Instruction fetched\n");
}
