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

typedef enum
{
	LEN_DEFAULT,
	LEN_LONG,
	LEN_LLONG,
	LEN_SIZE
} format_length_t;

void format(format_output_t* out, const char* fmt, va_list args);

int kvprintf(const char* fmt, va_list args);
int kprintf(const char* fmt, ...);

int vnsprintf(char* buffer, size_t size, const char* fmt, va_list args);
int snprintf(char* buffer, size_t size, const char* fmt, ...);

#endif
