#include <vm.h>

void dbg_printf(char* str)
{
	for(int i = 0; str[i] != '\0'; i++)
	{
		dbg_putc(str[i]);
	}
}

void dbg_print_hex(uint8_t h)
{
	char* foo = "00";
	char* hex = "0123456789ABCDEF";
	foo[0] = hex[(h >> 4) & 0xF];
	foo[1] = hex[h & 0xF];
	dbg_printf(foo);
}

void dbg_print_hex32(uint32_t h)
{
	dbg_print_hex((h >> 24) & 0xFF);
	dbg_print_hex((h >> 16) & 0xFF);
	dbg_print_hex((h >> 8) & 0xFF);
	dbg_print_hex( h & 0xFF);
}
