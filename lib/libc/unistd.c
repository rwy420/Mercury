#include "./unistd.h"
#include "./syscall.h"

void exit(int exit_code)
{
	syscall(0x01, exit_code, 0, 0, 0, 0);
}

pid_t fork()
{
	return syscall(0x02, 0, 0, 0, 0, 0);
}

int execve(const char* path, char* argv[], char* envp[])
{
	return syscall(0x0B, (uint32_t) path, (uint32_t) argv, (uint32_t) envp, 0, 0); 
}
