#ifndef __MERCURY__EXEC__USERMODE_H
#define __MERCURY__EXEC__USERMODE_H

#include <common/types.h>
#include <hardware/interrupts.h>


void execute_user_mode(void* entry);
void kernel_switch_back();
void syscall_exit(CPUState* cpu);

#endif
