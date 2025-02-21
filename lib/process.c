#include "./process.h"
#include "./syscall.h"

void exit(int exit_code)
{
	syscall(0x01, exit_code, 0, 0, 0, 0);
}
