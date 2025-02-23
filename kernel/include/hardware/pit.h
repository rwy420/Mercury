#ifndef __MERCURY__HARDWARE__PIT_H
#define __MERCURY__HARDWARE__PIT_H

#include <common/types.h>

void pit_init(uint32_t hz);
void pit_handle_interrupt();

#endif
