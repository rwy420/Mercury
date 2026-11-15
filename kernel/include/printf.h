#ifndef __PRINTF_H
#define __PRINTF_H

#include <stdarg.h>
#include <common/types.h>

typedef struct
{
	void (*putc)(char c);
	void* ctx;
	size_t count;
} format_output_t;

void format(format_output_t* out, char* fmt, va_list args);

int kvprintf(char* fmt, va_list args);
int kprintf(char* fmt, ...);

int vnsprintf(char* buffer, size_t size, char* fmt, va_list args);
int snprintf(char* buffer, size_t size, char* fmt, ...);

#endif
