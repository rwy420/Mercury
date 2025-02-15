#ifndef __QS__COMMON__SCREEN_H
#define __QS__COMMON__SCREEN_H

#include <common/types.h>
#include <hardware/interrupts.h>

void syscall_printf(CPUState* cpu);

void printf(string str);
void clear_screen();
void print_hex(uint8_t h);
void print_hex32(uint32_t h);

void print_uint8_t(uint8_t value);
void uint8_t_to_string(uint8_t value, uint8_t* buffer);

#endif
