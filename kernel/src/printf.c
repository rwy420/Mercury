#include <vesa.h>
#include <printf.h>

void format(format_output_t* out, char* fmt, va_list args)
{

}

int kprintf(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	format_output_t out = { vesa_putc_inc, 0, 0 };

	va_end(args);

	return 0;
}
