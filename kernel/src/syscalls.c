#include <syscalls.h>
#include <hardware/interrupts.h>
#include <common/screen.h>
#include <printf.h>

syscall_t handlers[256];

void syscall(CPUState* cpu)
{
	uint32_t syscall = cpu->eax;

	if(handlers[syscall] == NULL_PTR)
	{
		kprintf("Unhandled syscall 0x");
		print_hex32(syscall);
		kprintf("\n");
		return;
	}

	cpu->eax = handlers[syscall](cpu);
}

void register_syscall_handler(uint32_t syscall, syscall_t handler)
{
	if(syscall <= 0xFF) handlers[syscall] = handler;
}
