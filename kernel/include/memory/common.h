#ifndef __MERCURY__MEMORY__COMMON_H
#define __MERCURY__MEMORY__COMMON_H

#include <common/types.h>

void* memset(void* dest, uint8_t val, size_t len);
void* memmove(void* dest, const void* src, size_t len);
void* memcpy(void* dest, const void* src, size_t len);
int strcmp(const char *a, const char *b);
int memcmp(const void* s1, const void* s2, uint32_t size);

#endif
