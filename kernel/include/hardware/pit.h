#ifndef __MERCURY__HARDWARE__PIT_H
#define __MERCURY__HARDWARE__PIT_H

#include <hardware/interrupts.h>
#include <types.h>

void pit_init(uint32_t hz);
uint32_t pit_handle_interrupt(uint32_t esp);
void pit_set_schedule(int enable);
uint32_t ms_since_init();

#endif
