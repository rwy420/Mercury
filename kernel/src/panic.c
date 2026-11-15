#include <hardware/pit.h>
#include <vesa.h>
#include <printf.h>

void panic(char* reason, CPUState* cpu)
{
	pit_set_schedule(false);
	vesa_clear();
	kprintf("KERNEL PANIC: %s =(\n", reason);

	if(cpu)
	{
		kprintf("EIP: 0x%x\n", cpu->eip);
		kprintf("ESP: 0x%x\n", cpu->esp);
		kprintf("CPU ERROR: 0x%x\n", cpu->error_code);
		kprintf("EFLAGS: 0x%x\n", cpu->eflags);
		kprintf("CS: 0x%x / SS: 0x%x\n", cpu->cs, cpu->ss);
	}

	while(1) __asm__ __volatile__("sti; hlt");
}
