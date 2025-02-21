#include <driver/ps2/ps2keyboard.h>
#include <memory/mem_manager.h>
#include <hardware/interrupts.h>
#include <fs/fat16/fat16.h>

void handle_interrupt()
{
	uint8_t key = inb(DATA_PORT);
}

void ps2_kb_enable()
{
	register_interrupt_handler(0x21, handle_interrupt);

	while(inb(COMMAND_PORT) & 1) inb(DATA_PORT);

	outb(COMMAND_PORT, 0xAE);

	outb(COMMAND_PORT, 0x20);
	uint8_t status = (inb(DATA_PORT) | 1) & ~0x10;
	outb(COMMAND_PORT, 0x60);
	outb(DATA_PORT, status);
	outb(DATA_PORT, 0xF4);	
}

void ps2_kb_disable()
{
}
