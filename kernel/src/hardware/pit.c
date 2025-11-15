#include <hardware/pit.h>
#include <multitasking.h>
#include <common/screen.h>

volatile uint32_t g_ms_since_init;
uint32_t pit_hz;
int pit_schedule;

void pit_init(uint32_t hz)
{
	pit_hz = hz;
	g_ms_since_init = 0;

    int divisor = 1193180 / hz;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, divisor >> 8);

	pit_schedule = 0;
	register_interrupt_handler(0x20, pit_handle_interrupt);
}

void pit_handle_interrupt(CPUState* cpu)
{
	g_ms_since_init += 1000 / pit_hz;

	if(pit_schedule) schedule(cpu);
}

void pit_set_schedule(int enable)
{
	pit_schedule = enable;
}

uint32_t ms_since_init()
{
	return g_ms_since_init;
}
