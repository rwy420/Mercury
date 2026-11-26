#ifndef __MERCURY__PROCESS_H
#define __MERCURY__PROCESS_H

#include <hardware/interrupts.h>

void syscall_exit(CPUState* cpu);
int process_create(const char* path, int argc, char** argv);

#endif
