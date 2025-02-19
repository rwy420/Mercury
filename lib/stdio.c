#include "./stdio.h"

void printf(char* msg)
{
	unsigned int syscall = 1;

	__asm__ volatile(
		"movl %0, %%eax\n"
		"movl %1, %%ebx\n"
		"int $0x80\n"
		:
		: "r" (syscall), "r" (msg)
		: "eax", "ebx"
	);
}
