#ifndef __MERCURY__MEMORY__COMMON_H
#define __MERCURY__MEMORY__COMMON_H

#include <types.h>

void* memset(void* dest, uint8_t val, size_t len);
void* memmove(void* dest, const void* src, size_t len);
void* memcpy(void* dest, const void* src, size_t len);
int strcmp(const char* a, const char* b);
size_t strlen(const char* str);
int memcmp(const void* s1, const void* s2, uint32_t size);

void uint8_t_to_string(uint8_t value, char* buffer);
void uint32_t_to_string(uint32_t value, char* buffer);

void to_uppercase(char* str, size_t len);

int atomic_compare_exchange(int* ptr, int* expected, int desired);

#endif
