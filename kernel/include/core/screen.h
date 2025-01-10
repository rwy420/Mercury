#ifndef __MERCURY__CORE__SCREEN_H
#define __MERCURY__CORE__SCREEN_H

#include <core/types.h>

void printf(string str);
void clear_screen();
void print_hex(uint8_t h);
void print_hex32(uint32_t h);

void print_uint8_t(uint8_t value);
void uint8_t_to_string(uint8_t value, uint8_t* buffer);

#endif
