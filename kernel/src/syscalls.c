#include <syscalls.h>
#include <hardware/interrupts.h>
#include <core/screen.h>

void handle_syscall(CPUState* cpu)
{	
	switch(cpu->eax)
	{
		case 4:
			printf((char*) cpu->ebx);
			break;
		default:
			break;
	}
}
