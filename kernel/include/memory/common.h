#ifndef __MERCURY__MEMORY__COMMON_H
#define __MERCURY__MEMORY__COMMON_H

#include <common/types.h>

void* memset(void* buffer, uint8_t byte, uint32_t length);
void* memmove(void* dest, const void* src, size_t len);
void memcpy(void* dest, void* src, size_t n);
int strcmp(const char *a, const char *b);
int memcmp(const void* s1, const void* s2, uint32_t size);

#endif
