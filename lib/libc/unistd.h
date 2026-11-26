#ifndef __UNISTD_H
#define __UNISTD_H

#include "./types.h"

void exit(int exit_code);
pid_t fork();
int execve(const char* path, char* argv[], char* envp[]);

#endif
