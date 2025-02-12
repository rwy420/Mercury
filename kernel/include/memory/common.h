#ifndef __QS__MEMORY__COMMON_H
#define __QS__MEMORY__COMMON_H

#include <core/types.h>

void* memset(void* buffer, uint8_t byte, uint32_t length);
void* memmove(void* dest, const void* src, size_t len);
void memcpy(void* dest, void* src, size_t n);
bool strcmp(const char *a, const char *b);
int memcmp(const void* s1, const void* s2, uint32_t size);
void print_memory_info();

#endif
