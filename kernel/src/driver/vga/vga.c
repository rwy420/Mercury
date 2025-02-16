#include <driver/vga/vga.h>
#include <hardware/port.h>
#include <memory/common.h>

unsigned char g_720x480x16[] =
{
/* MISC */
	0xE7,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x6B, 0x59, 0x5A, 0x82, 0x60, 0x8D, 0x0B, 0x3E,
	0x00, 0x40, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x2D, 0x08, 0xE8, 0x05, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x01, 0x00, 0x0F, 0x00, 0x00,
};

uint8_t* vga_memory = (uint8_t*)0xA0000;

void vga_write_registers()
{
	uint8_t* registers = g_720x480x16;

	outb(MISC_PORT, *(registers++));

	for(uint8_t i = 0; i < 5; i++)
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

	for(uint8_t i = 0; i < 25; i++)
	{
		outb(CRTC_INDEX_PORT, i);
		outb(CRTC_DATA_PORT, *(registers++));
	}

	for(uint8_t i = 0; i < 9; i++)
	{
		outb(CONTROLLER_INDEX_PORT, i);
		outb(CONTROLLER_DATA_PORT, *(registers++));
	}

	for(uint8_t i = 0; i < 21; i++)
	{
		inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
		outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, i);
		outb(ATTRIBUTE_CONTROLLER_WRITE_PORT, *(registers++));
	}

	inb(ATTRIBUTE_CONTROLLER_RESET_PORT);
	outb(ATTRIBUTE_CONTROLLER_INDEX_PORT, 0x20);
}

void vga_set_plane(uint8_t p)
{
	static unsigned curr_p = -1u;
	uint8_t pmask;

	p &= 3;
	if(p == curr_p)
		return;
	curr_p = p;
	pmask = 1 << p;

	outw(CONTROLLER_INDEX_PORT, (p << 8) | 4);
	outw(SEQUENCER_INDEX_PORT, (pmask << 8) | 2);
}

void vga_set_pixel(int x, int y, uint8_t color)
{
	uint32_t wd_in_bytes, off, mask, p, pmask;

	wd_in_bytes = VGA_WIDTH / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	uint8_t* mem = (uint8_t*) vga_memory + off;
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


void vga_fill(uint8_t color) {
	for (int plane = 0; plane < 4; plane++) {
        vga_set_plane(plane);
        uint8_t fill_value = (color & (1 << plane)) ? 0xFF : 0x00;
        memset((void*)vga_memory, fill_value, VGA_WIDTH * VGA_HEIGHT / 8);
    }
}

void draw_rectangle(int x, int y, int w, int h, uint8_t color) {
    for (int plane = 0; plane < 4; plane++) {
        vga_set_plane(plane);
        for (int dy = 0; dy < h; dy++) {
            for (int dx = 0; dx < w; dx++) {
                int px = x + dx;
                int py = y + dy;
                uint8_t* fb_byte_ptr = (uint8_t*)vga_memory + ((py * 720 + px) >> 3);
                uint8_t bit_mask = (1 << (7 - (px & 0x07)));
                uint8_t b = *fb_byte_ptr;

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

	draw_rectangle(10, 10, 8, 8, 4);
}
