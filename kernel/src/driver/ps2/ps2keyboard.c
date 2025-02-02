#include <driver/ps2/ps2keyboard.h>

#define DATA_PORT 0x60
#define COMMAND_PORT 0x64

void ps2_kb_enable()
{
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

void ps2_kb_handle_interrupt()
{
	uint8_t key = inb(DATA_PORT);
	printf("KEY");
}
