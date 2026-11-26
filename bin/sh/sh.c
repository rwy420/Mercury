#include <sh.h>
#include <stdio.h>
#include <types.h>
#include <unistd.h>
#include <wait.h>

int main()
{
	char command[MAX_COMMAND_LEN];

	for(;;)
	{
		write(1, "# ", 2);
		size_t len = read(0, command, MAX_COMMAND_LEN);

		break; // until read on stdin works so this doesn't print '#' infinitely

		if(len <= 1) continue;

		pid_t fork_result = fork();

		if(fork_result == 0)
		{
			execve(command, 0, 0);
			break;
		}
		else
		{
			wait4(-1, 0, 0, 0);
		}
	}

	return 0;
}
