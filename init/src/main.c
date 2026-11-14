#include <syscall.h>

char hello[] = "Hello World from init\n";

int global = 40;
int* ptr = &global;

int main(int argc, char** argv)
{
	// write message to stdout
	syscall(0x04, 0x01, (unsigned int) hello, sizeof(hello), 0, 0);
	char* arg1 = argv[0];

	syscall(0x04, 0x01, (unsigned int) arg1, 0, 0, 0);

	int val = *ptr;

	return 69;
}
