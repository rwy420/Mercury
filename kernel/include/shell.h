#ifndef __MERCURY__SHELL_H
#define __MERCURY__SHELL_H

#include <common/types.h>

void shell_init();
void shell_putc(uint8_t key);
void shell_exec();

#endif
