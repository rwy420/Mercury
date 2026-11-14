#include <hardware/pit.h>
#include <common/screen.h>
#include <vesa.h>

void panic(char* reason, CPUState* cpu)
{
	pit_set_schedule(false);
	clear_screen();
	printf_color("KERNEL PANIC: ", COLOR_RED, COLOR_BLACK);
	printf_color(reason, COLOR_RED, COLOR_BLACK);
	printf_color(" =(\n", COLOR_RED, COLOR_BLACK);

	if(cpu)
	{
		printf("EIP: 0x");
		print_hex32(cpu->eip);
		printf("\nESP: 0x");
		print_hex32(cpu->esp);
		printf("\nCPU ERROR: 0x");
		print_hex32(cpu->error_code);
		printf("\nEFLAGS: 0x");
		print_hex32(cpu->eflags);
		printf("\nCS / SS: 0x");
		print_hex32(cpu->cs);
		printf("/0x");
		print_hex32(cpu->ss);
		printf("\n");
	}

	while(1) __asm__ __volatile__("sti; hlt");
}
