#ifndef __MERCURY__SYSCALLS_H
#define __MERCURY__SYSCALLS_H

#include <hardware/interrupts.h>
#include <common/types.h>

typedef int (*syscall_t)(CPUState* cpu);

int syscall(CPUState* cpu);
void register_syscall_handler(uint32_t syscall, syscall_t handler);

#endif
