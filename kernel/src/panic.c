#include <hardware/pit.h>
#include <common/screen.h>
#include <vesa.h>
#include <printf.h>

void panic(char* reason, CPUState* cpu)
{
	pit_set_schedule(false);
	vesa_clear();
	kprintf("KERNEL PANIC: ");
	kprintf(reason);
	kprintf(" =(\n");

	if(cpu)
	{
		kprintf("EIP: 0x");
		print_hex32(cpu->eip);
		kprintf("\nESP: 0x");
		print_hex32(cpu->esp);
		kprintf("\nCPU ERROR: 0x");
		print_hex32(cpu->error_code);
		kprintf("\nEFLAGS: 0x");
		print_hex32(cpu->eflags);
		kprintf("\nCS / SS: 0x");
		print_hex32(cpu->cs);
		kprintf("/0x");
		print_hex32(cpu->ss);
		kprintf("\n");
	}

	while(1) __asm__ __volatile__("sti; hlt");
}
