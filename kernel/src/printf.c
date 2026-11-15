#include <vesa.h>
#include <printf.h>

static void emit(format_output_t* out, char c)
{
	out->putc(c);
	out->count++;
}

static void emit_string(format_output_t* out, char* str)
{
	while(*str)
	{
		out->putc(*str++);
		out->count++;
	}
}

void format(format_output_t* out, char* fmt, va_list args)
{
	while(*fmt)
	{
		if(*fmt != '%')
		{
			emit(out, *fmt++);
			continue;
		}

		fmt++;

		if(*fmt == '%')
		{
			emit(out, '%');
			fmt++;
			continue;
		}

		// Length modifier
		format_length_t len = LEN_DEFAULT;

		if(*fmt == 'l')
		{
			fmt++;

			if(*fmt == 'l')
			{
				len = LEN_LLONG;
				fmt++;
			}
			else len = LEN_LONG;
		}
		else if(*fmt == 'z')
		{
			len = LEN_SIZE;
			fmt++;
		}

		switch(*fmt)
		{
			case 'c':
				emit(out, (char) va_arg(args, int));
				break;

			case 's':
				char* str = va_arg(args, char*);
				emit_string(out, str ? str : "(null)");
				break;
		}

		if(*fmt) fmt++;
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
