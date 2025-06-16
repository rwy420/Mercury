#ifndef __MERCURY__COMMON__SCREEN_H
#define __MERCURY__COMMON__SCREEN_H

#include <common/types.h>
#include <hardware/interrupts.h>

int syscall_printf(void* buffer, size_t length);

void printf_color(string str, uint32_t fg, uint32_t bg);
void printf(string str);
void terminal_move_left();
void clear_screen();
void print_hex(uint8_t h);
void print_hex32(uint32_t h);

void print_uint8_t(uint8_t value);
void print_uint32_t(uint32_t value);
void uint8_t_to_string(uint8_t value, char* buffer);
void uint32_t_to_string(uint32_t value, char* buffer);

#endif
