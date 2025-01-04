#ifndef __MERCURY__MEMORY__COMMON_H
#define __MERCURY__MEMORY__COMMON_H

#include <core/types.h>

void* memset(void* buffer, uint8_t byte, uint32_t length);
void* memmove(void* dest, const void* src, size_t len);
bool strcmp(const char *a, const char *b);
void print_memory_info();

#endif
