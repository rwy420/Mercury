#include <vm.h>
#include <stdarg.h>
#include <printf.h>
#include <memory/common.h>

void dbgprintf(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	// Add 'DEBUG: ' as prefix to debug message with ansi escape codes to print in magenta
	char dbg[strlen(fmt) + 16];
	memcpy(dbg, "\033[35mDEBUG: \033[0m ", 16);
	memcpy(dbg + 16, fmt, strlen(fmt) + 1);

	format_output_t out = { dbg_putc, 0, 0 };
	format(&out, dbg, args);

	va_end(args);
}
