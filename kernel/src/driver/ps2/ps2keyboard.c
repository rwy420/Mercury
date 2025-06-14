#define DATA_PORT 0x60
#define COMMAND_PORT 0x64

#include <driver/ps2/ps2keyboard.h>
#include <memory/heap.h>
#include <hardware/interrupts.h>
#include <fs/fat16/fat16.h>
#include <common/screen.h>
#include <shell.h>

int shift;

void handle_interrupt()
{
	uint8_t key = inb(DATA_PORT);
	print_hex(key);

	/*if(key == 0x2A || key == 0x36) shift = true;
	if(key == 0xAA || key == 0xB6) shift = false;

	if(key == 0x0E)
	{
		shell_putc('\b');
		return;
	}


    if(key < 0x80)
    {
        switch(key)
        {
            case 0x02: if(shift) shell_putc('!'); else shell_putc('1'); break;
            case 0x03: if(shift) shell_putc('\"'); else shell_putc('2'); break;
            case 0x04: if(shift) shell_putc(' '); else shell_putc('3'); break;
            case 0x05: if(shift) shell_putc('$'); else shell_putc('4'); break;
            case 0x06: if(shift) shell_putc('%'); else shell_putc('5'); break;
            case 0x07: if(shift) shell_putc('&'); else shell_putc('6'); break;
            case 0x08: if(shift) shell_putc('/'); else shell_putc('7'); break;
            case 0x09: if(shift) shell_putc('('); else shell_putc('8'); break;
            case 0x0A: if(shift) shell_putc(')'); else shell_putc('9'); break;
            case 0x0B: if(shift) shell_putc('='); else shell_putc('0'); break;

            case 0x10: if(shift) shell_putc('Q'); else shell_putc('q'); break;
            case 0x11: if(shift) shell_putc('W'); else shell_putc('w'); break;
            case 0x12: if(shift) shell_putc('E'); else shell_putc('e'); break;
            case 0x13: if(shift) shell_putc('R'); else shell_putc('r'); break;
            case 0x14: if(shift) shell_putc('T'); else shell_putc('t'); break;
            case 0x15: if(shift) shell_putc('Z'); else shell_putc('z'); break;
            case 0x16: if(shift) shell_putc('U'); else shell_putc('u'); break;
            case 0x17: if(shift) shell_putc('I'); else shell_putc('i'); break;
            case 0x18: if(shift) shell_putc('O'); else shell_putc('o'); break;
            case 0x19: if(shift) shell_putc('P'); else shell_putc('p'); break;

            case 0x1E: if(shift) shell_putc('A'); else shell_putc('a'); break;
            case 0x1F: if(shift) shell_putc('S'); else shell_putc('s'); break;
            case 0x20: if(shift) shell_putc('D'); else shell_putc('d'); break;
            case 0x21: if(shift) shell_putc('F'); else shell_putc('f'); break;
            case 0x22: if(shift) shell_putc('G'); else shell_putc('g'); break;
            case 0x23: if(shift) shell_putc('H'); else shell_putc('h'); break;
            case 0x24: if(shift) shell_putc('J'); else shell_putc('j'); break;
            case 0x25: if(shift) shell_putc('K'); else shell_putc('k'); break;
            case 0x26: if(shift) shell_putc('L'); else shell_putc('l'); break;

            case 0x2C: if(shift) shell_putc('Y'); else shell_putc('y'); break;
            case 0x2D: if(shift) shell_putc('X'); else shell_putc('x'); break;
            case 0x2E: if(shift) shell_putc('C'); else shell_putc('c'); break;
            case 0x2F: if(shift) shell_putc('V'); else shell_putc('v'); break;
            case 0x30: if(shift) shell_putc('B'); else shell_putc('b'); break;
            case 0x31: if(shift) shell_putc('N'); else shell_putc('n'); break;
            case 0x32: if(shift) shell_putc('M'); else shell_putc('m'); break;
            case 0x33: if(shift) shell_putc(';'); else shell_putc(','); break;
			case 0x34: if(shift) shell_putc(':'); else shell_putc('.'); break;
            case 0x35: if(shift) shell_putc('_'); else shell_putc('-'); break;

            case 0x1C: shell_exec(); break;
            case 0x39: shell_putc(' '); break;

			default:
            {
                break;
            }
        }
    }*/
}

void ps2_kb_enable()
{
	shift = false;
	register_interrupt_handler(0x21, (isr_t) handle_interrupt);

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
