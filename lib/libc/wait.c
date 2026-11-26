#include "./wait.h"
#include "./syscall.h"

pid_t wait4(pid_t pid, int wstatus, int options, void* rusage)
{
	return syscall(0x72, wstatus, options, (uint32_t) rusage, 0, 0);
}
