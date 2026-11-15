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

static void emit_unsigned(format_output_t* out, uint32_t value, uint32_t base, int uppercase)
{
	char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
	char buffer[32];
	size_t i = 0;

	do {
		buffer[i++] = digits[value % base];
		value /= base;
	}while (value);

	while(i) emit(out, buffer[--i]);
}

//TODO: support 64bit values by implementing a custom '__udivmoddi4' for 64bit division
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
			case 'c': // Character
				emit(out, (char) va_arg(args, int));
				break;

			case 's': // String
				char* str = va_arg(args, char*);
				emit_string(out, str ? str : "(null)");
				break;

			case 'd': //TODO Signed integer type
			case 'i': // Signed integer
				int32_t value;

				switch(len)
				{
					case LEN_LONG:
						value = va_arg(args, long);
						break;

					/*case LEN_LLONG:
						value = va_arg(args, int64_t);
						break;*/

					case LEN_SIZE:
						value = va_arg(args, int32_t);
						break;

					default:
						value = va_arg(args, int32_t);
						break;
				}

				uint32_t mag;

				if(value < 0)
				{
					emit(out, '-');
					mag = -(uint32_t) value;
				}
				else mag = (uint32_t) value;

				emit_unsigned(out, mag, 10, 0);
				break;

			case 'u': // Unsgined integer
				uint32_t uvalue;

				switch(len)
				{
					case LEN_LONG:
						uvalue = va_arg(args, unsigned long);
						break;

					/*case LEN_LLONG:
						uvalue = va_arg(args, uint64_t);
						break;*/

					case LEN_SIZE:
						uvalue = va_arg(args, uint32_t);
						break;

					default:
						uvalue = va_arg(args, uint32_t);
						break;
				}

				emit_unsigned(out, uvalue, 10, 0);
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
