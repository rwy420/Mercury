#ifndef BOOT__SCREEN_H
#define BOOT__SCREEN_H

#define MAX_X 80
#define MAX_Y 25

#include "./types.h"

void printf(string str);
void print_hex(uint8_t h);
void print_hex32(uint32_t h);
void clear_screen();


#endif
