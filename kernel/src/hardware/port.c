#include <hardware/port.h>

uint8_t inb(uint16_t port)
{
	uint8_t result;
	__asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void outb(uint16_t port, uint8_t data)
{
	__asm__ volatile("outb %0, %1" : : "a" (data), "Nd" (port));
}

void outb_slow(uint16_t port, uint8_t data)
{
	__asm__ volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:" : : "a" (data), "Nd" (port));
}

uint16_t inw(uint16_t port)
{
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void outw(uint16_t port, uint16_t data)
{
	__asm__ volatile("outw %0, %1" : : "a" (data), "Nd" (port));
}

uint32_t inl(uint16_t port)
{
	uint32_t result;
	__asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}

void outl(uint16_t port, uint32_t data)
{
	__asm__ volatile("outl %0, %1" : : "a" (data), "Nd" (port));
}

void io_wait()
{
	__asm__ volatile("outb %%al, $0x80" : : "a" (0));
}
