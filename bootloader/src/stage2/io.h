#ifndef BOOT__IO_H
#define BOOT__IO_H

#include "./types.h"

static inline uint8_t inb(uint16_t port)
{
	uint8_t result;
	__asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

static inline void outb(uint16_t port, uint8_t data)
{
	__asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

static inline uint16_t inw(uint16_t port)
{
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

static inline void outw(uint16_t port, uint16_t data)
{
	__asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

#endif
