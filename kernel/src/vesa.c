#include "common/screen.h"
#include <vesa.h>
#include <psf1.h>
#include <memory/common.h>
#include <memory/paging.h>

extern VesaInfoBlock g_vesa_info_block;

uint32_t* vesa_fb;

void vesa_init()
{
	vesa_fb = (uint32_t*) g_vesa_info_block.fb;
}

void vesa_put_pixel(int x, int y, uint16_t color)
{
	vesa_fb[y * g_vesa_info_block.fb_width + y] = color;
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
            int index = pixel_y * g_vesa_info_block.fb_width + pixel_x;

            if (bits & (1 << (7 - col))) vesa_fb[index] = fg;
            else vesa_fb[index] = bg;
        }
    }
}

void vesa_clear()
{
	memset(vesa_fb, 0x0, g_vesa_info_block.fb_height * g_vesa_info_block.fb_width);
}

void vesa_map(PageDirectory* pd)
{
	for(int i = 0; i  < g_vesa_info_block.fb_height * g_vesa_info_block.fb_width * 2; i += PAGE_SIZE)
	{
		void* address = (void*) (g_vesa_info_block.fb + i);
		map_page_pd(pd, address, address);
	}
}
