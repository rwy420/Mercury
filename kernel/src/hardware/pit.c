#include <hardware/pit.h>
#include <hardware/interrupts.h>

void pit_init(uint32_t hz)
{
	register_interrupt_handler(0x20, pit_handle_interrupt);

    int divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);
}

void pit_handle_interrupt()
{
}
