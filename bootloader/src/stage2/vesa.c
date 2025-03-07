#include "./vesa.h"
#include "./font.h"
#include "./common.h"

extern uint32_t vesa_data[4];
uint32_t vesa_width, vesa_height;

uint32_t framebuffer;
uint16_t* vesa_fb;

void vesa_init()
{
	framebuffer = vesa_data[0];
	vesa_width = 1024;
	vesa_height = 768;
	vesa_fb = (uint16_t*) framebuffer;
}

void vesa_put_pixel(int x, int y, uint16_t color)
{
	vesa_fb[y * vesa_width + x] = color;
}

void vesa_putc(char c, int x, int y, uint16_t fg, uint16_t bg)
{
	uint8_t *glyph = ((uint8_t*)(&u_vga16_psf) + sizeof(PSF1Header)) + (c * 0xE);
	
	for (int row = 0; row < 0xE; row++)
	{
        uint8_t bits = glyph[row];

        for (int col = 0; col < 8; col++)
		{
            int pixel_x = x + col;
            int pixel_y = y + row;
            int index = pixel_y * vesa_width + pixel_x;

            if (bits & (1 << (7 - col))) vesa_fb[index] = fg;
            else vesa_fb[index] = bg;
        }
    }
}

void vesa_clear()
{
	memset(vesa_fb, 0x0, vesa_height * vesa_width);
}
