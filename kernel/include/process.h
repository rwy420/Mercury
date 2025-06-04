#ifndef __MERCURY__PROCESS_H
#define __MERCURY__PROCESS_H

#include <hardware/interrupts.h>

void syscall_exit(CPUState* cpu);
uint32_t create_user_process_pd(void* code);

#endif
