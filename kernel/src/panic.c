#include <hardware/pit.h>
#include <common/screen.h>
#include <vesa.h>

void panic(char* reason, CPUState* cpu)
{
	pit_set_schedule(false);
	vesa_clear();
	printf("KERNEL PANIC: ");
	printf(reason);
	printf(" =(\n");

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
