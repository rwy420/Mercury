#include <core/screen.h>

#define MAX_X 80
#define MAX_Y 25

static uint8_t x = 0, y = 0;

/*void sysprintf(string str)
{
	asm("int $0x80" : : "a" (4), "b" (str));
}*/

void printf(string str)
{
	uint16_t* video_memory = (uint16_t*) 0xb8000;
	for(int i = 0; str[i] != '\0'; i++)
	{

		switch(str[i])
		{
			case '\n':
			{
				y++;
				x = 0;
				break;
			}

			default:
			{
				video_memory[MAX_X * y + x] = (video_memory[MAX_X * y + x] & 0xFF00) | str[i];
				x++;
				break;
			}
		}

		if(x >= MAX_X)
		{
			y++;
			x = 0;
		}

		if(y >= 25)
		{
			clear_screen();
		}
	}
}

void clear_screen()
{
	uint16_t* video_memory = (uint16_t*) 0xb8000;

	for(y = 0; y < MAX_Y; y++)
	{
		for(x = 0; x < MAX_X; x++)
		{ 
			video_memory[MAX_X * y + x] = (video_memory[MAX_X * y + x] & 0xFF00) | ' ';
		}
	}

	x = 0;
	y = 0;
}

void print_hex(uint8_t h)
{
	char* foo = "00";
	char* hex = "0123456789ABCDEF";
	foo[0] = hex[(h >> 4) & 0xF];
	foo[1] = hex[h & 0xF];
	printf(foo);
}

void print_hex32(uint32_t h)
{
	print_hex((h >> 24) & 0xFF);
	print_hex((h >> 16) & 0xFF);
	print_hex((h >> 8) & 0xFF);
	print_hex( h & 0xFF);
}
