#ifndef __MERCURY__DRIVERS__VGA_H
#define __MERCURY__DRIVERS__VGA_H

void vga_write_registers();
void vga_set_plane(unsigned char plane);
void vga_set_pixel(int x, int y, unsigned char color);
void draw_rectangle(int x, int y, int w, int h, unsigned char color);
void vga_fill(unsigned char color);
void vga_init();

#endif
