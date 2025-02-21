#include <syscalls.h>
#include <hardware/interrupts.h>
#include <common/screen.h>

syscall_t handlers[256];

int syscall(CPUState* cpu)
{
	uint32_t syscall = cpu->eax;

	if(handlers[syscall] == NULL_PTR)
	{
		printf("Unhandled syscall 0x");
		print_hex32(syscall);
		printf("\n");
		return 0x0;
	}

	return handlers[syscall](cpu);
}

void register_syscall_handler(uint32_t syscall, syscall_t handler)
{
	if(syscall <= 0xFF) handlers[syscall] = handler;
}
