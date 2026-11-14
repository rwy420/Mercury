#ifndef __MERCURY__PANIC_H
#define __MERCURY__PANIC_H

#include <hardware/interrupts.h>

void panic(char* reason, CPUState* cpu);

#endif
