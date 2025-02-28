#include <hardware/pic.h>

uint32_t pic_offset1, pic_offset2;

void pic_confirm(uint32_t interrupt)
{
	if(interrupt < pic_offset1 || interrupt > 0x27)
	{
		return;
	}

	if(interrupt <pic_offset2)
	{
		outb(PIC_1_COMMAND, PIC_CONFIRM);
	}
	else
	{
		outb(PIC_2_COMMAND, PIC_CONFIRM);
	}
}

void pic_remap(uint32_t offset1, uint32_t offset2)
{
	pic_offset1 = offset1;
	pic_offset2 = offset2;

	outb(PIC_1_COMMAND, 0x11);
	io_wait();
	outb(PIC_2_COMMAND, 0x11);
	io_wait();

	outb(PIC_1_DATA, offset1);
	io_wait();
	outb(PIC_2_DATA, offset2);
	io_wait();

	outb(PIC_1_DATA, 0x04);
	io_wait();
	outb(PIC_2_DATA, 0x02);
	io_wait();

	outb(PIC_1_DATA, PIC_ICW4_8086);
	io_wait();
	outb(PIC_2_DATA, PIC_ICW4_8086);
	io_wait();

	outb(PIC_1_DATA, 0xFC);
	io_wait();
	outb(PIC_2_DATA, 0xFF);
	io_wait();
}
