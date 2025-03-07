#ifndef BOOT__VESA_H
#define BOOT__VESA_H

#include "./types.h"

void vesa_init();
void vesa_put_pixel(int x, int y, uint16_t color);
void vesa_putc(char c, int x, int y, uint16_t fg, uint16_t bg);
void vesa_clear();

#endif
