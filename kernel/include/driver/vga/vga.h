#ifndef __MERCURY__DRIVER__VGA__VGA_H
#define __MERCURY__DRIVER__VGA__VGA_H

#include <common/types.h>

#define VGA_WIDTH 720
#define VGA_HEIGHT 480

void vga_write_registers();
void vga_set_plane(uint8_t plane);
void vga_set_pixel(int x, int y, uint8_t color);
void draw_rectangle(int x, int y, int w, int h, uint8_t color);
void vga_fill(uint8_t color);
void vga_init();

#endif
