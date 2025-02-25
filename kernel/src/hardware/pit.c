#include <hardware/pit.h>
#include <hardware/interrupts.h>
#include <multitasking.h>

void pit_init(uint32_t hz)
{
    int divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}
