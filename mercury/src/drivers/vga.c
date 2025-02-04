#include <drivers/vga.h>
#include <qslibc/qs_io.h>
#include <qslibc/qs_mem.h>

#define VGA_WIDTH 640
#define VGA_HEIGHT 480

#define MISC_PORT 0x3C2
#define CRTC_INDEX_PORT 0x3D4
#define CRTC_DATA_PORT 0x3D5
#define SEQUENCER_INDEX_PORT 0x3C4
#define SEQUENCER_DATA_PORT 0x3C5
#define CONTROLLER_INDEX_PORT 0x3CE
#define CONTROLLER_DATA_PORT 0x3CF
#define ATTRIBUTE_CONTROLLER_INDEX_PORT 0x3C0
#define ATTRIBUTE_CONTROLLER_READ_PORT 0x3C1
#define ATTRIBUTE_CONTROLLER_WRITE_PORT 0x3C0
#define ATTRIBUTE_CONTROLLER_RESET_PORT 0x3DA

unsigned char g_640x480x16[61] =
{
/* MISC */
	0xE3,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

unsigned char* vga_memory = (unsigned char*)0xA0000;

void vga_write_registers()
{
	unsigned char* registers = g_640x480x16;

	outb(MISC_PORT, *(registers++));

	for(unsigned char i = 0; i < 5; i++)
	{
		outb(SEQUENCER_INDEX_PORT, i);
		outb(SEQUENCER_DATA_PORT, *(registers++));
	}

	outb(CRTC_INDEX_PORT, 0x03);
	outb(CRTC_DATA_PORT, inb(CRTC_DATA_PORT) | 0x80);
	outb(CRTC_INDEX_PORT, 0x11);
	outb(CRTC_DATA_PORT, inb(CRTC_DATA_PORT) & ~0x80);

	registers[0x03] = registers[0x03] | 0x80;
	registers[0x11] = registers[0x11] & ~0x80;

	for(unsigned char i = 0; i < 25; i++)
	{
		outb(CRTC_INDEX_PORT, i);
		outb(CRTC_DATA_PORT, *(registers++));
	}

	for(unsigned char i = 0; i < 9; i++)
	{
		outb(CONTROLLER_INDEX_PORT, i);
		outb(CONTROLLER_DATA_PORT, *(registers++));
	}

	for(unsigned char i = 0; i < 21; i++)
	{
		inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
		outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, i);
		outb(ATTRIBUTE_CONTROLLER_WRITE_PORT, *(registers++));
	}

	inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
	outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, 0x20);
}

void vga_set_plane(unsigned char p)
{
	static unsigned curr_p = -1u;
	unsigned char pmask;

	p &= 3;
	if(p == curr_p)
		return;
	curr_p = p;
	pmask = 1 << p;

	outw(CONTROLLER_INDEX_PORT, (p << 8) | 4);
	outw(SEQUENCER_INDEX_PORT, (pmask << 8) | 2);
}

void vga_set_pixel(int x, int y, unsigned char color)
{
	unsigned int wd_in_bytes, off, mask, p, pmask;

	wd_in_bytes = VGA_WIDTH / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	unsigned char* mem = (unsigned char*) vga_memory + off;
	for(p = 0; p < 4; p++)
	{
		vga_set_plane(p);
		if(pmask & color)
			*mem |= mask;
		else
			*mem &= ~mask;
		pmask <<= 1;
	}
}


void vga_fill(unsigned char color) {
	for (int plane = 0; plane < 4; plane++) {
        vga_set_plane(plane);
        unsigned char fill_value = (color & (1 << plane)) ? 0xFF : 0x00;
        memset((void*)vga_memory, fill_value, VGA_WIDTH * VGA_HEIGHT / 8);
    }
}

void draw_rectangle(int x, int y, int w, int h, unsigned char color) {
    for (int plane = 0; plane < 4; plane++) {
        vga_set_plane(plane);
        for (int dy = 0; dy < h; dy++) {
            for (int dx = 0; dx < w; dx++) {
                int px = x + dx;
                int py = y + dy;
                unsigned char* fb_byte_ptr = (unsigned char*)vga_memory + ((py * 640 + px) >> 3);
                unsigned char bit_mask = (1 << (7 - (px & 0x07)));
                unsigned char b = *fb_byte_ptr;

                if (color & (1 << plane)) {
                    b |= bit_mask;
                } else {
                    b &= ~bit_mask;
                }

               *fb_byte_ptr = b;
            }
        }
    }
}

void vga_init()
{
	vga_write_registers();
	vga_fill(0x1);
}
