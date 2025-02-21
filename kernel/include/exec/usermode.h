#ifndef __MERCURY__EXEC__USERMODE_H
#define __MERCURY__EXEC__USERMODE_H

#include <common/types.h>

void execute_user_mode(void* entry);
void kernel_switch_back();
void syscall_exit(int status);

#endif
