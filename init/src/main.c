#include <syscall.h>

char hello[] = "Hello World from init";

int main()
{
	// write message to stdout
	syscall(0x04, 0x01, (unsigned int) hello, sizeof(hello), 0, 0);
	// Privilege test
	asm volatile("hlt");

	while(1);
}
