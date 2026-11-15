#include <vesa.h>
#include <printf.h>

static void emit(format_output_t* out, char c)
{
	out->putc(c);
	out->count++;
}

void format(format_output_t* out, char* fmt, va_list args)
{
	while(*fmt)
	{
		emit(out, *fmt++);
		continue;
	}
}

int kprintf(char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	format_output_t out = { vesa_putc_inc, 0, 0 };
	format(&out, fmt, args);

	va_end(args);

	return out.count;
}
