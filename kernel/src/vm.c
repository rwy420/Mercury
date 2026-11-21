#include <vm.h>
#include <stdarg.h>
#include <printf.h>

void dbg_printf(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	format_output_t out = { dbg_putc, 0, 0 };
	format(&out, fmt, args);

	va_end(args);
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
