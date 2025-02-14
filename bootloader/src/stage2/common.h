#ifndef BOOT__COMMON_H
#define BOOT__COMMON_H

#include "./types.h"

void* memset(void* buffer, uint8_t byte, uint32_t length);
void memcpy(void* dest, void* src, uint32_t n);
bool strcmp(const char* a, const char* b);
int memcmp(const void* s1, const void* s2, uint32_t size);

#endif
