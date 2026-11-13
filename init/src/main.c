#include <syscall.h>

char hello[] = "Hello World from init\n";

int global = 40;
int* ptr = &global;

int main()
{
	// write message to stdout
	syscall(0x04, 0x01, (unsigned int) hello, sizeof(hello), 0, 0);

	int val = *ptr;

	while(1);
}
