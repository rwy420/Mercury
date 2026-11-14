#include <syscall.h>

char hello[] = "Hello World from init\n";

int global = 40;
int* ptr = &global;

int main(int argc)
{
	// write message to stdout
	syscall(0x04, 0x01, (unsigned int) hello, sizeof(hello), 0, 0);
	char* argcstr = " \n\0";
	argcstr[0] = '0' + argc;

	syscall(0x04, 0x01, (unsigned int) argcstr, sizeof(argcstr), 0, 0);

	int val = *ptr;

	while(1);
}
