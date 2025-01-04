#include <hardware/pic.h>

#define PIC_1		0x20		
#define PIC_2		0xA0		
#define PIC_1_COMMAND	PIC_1
#define PIC_1_DATA	(PIC_1+1)
#define PIC_2_COMMAND	PIC_2
#define PIC_2_DATA	(PIC_2+1)


#define PIC_CONFIRM 0x20

#define PIC_ICW1_ICW4            0x01	
#define PIC_ICW1_SINGLE          0x02	
#define PIC_ICW1_INTERVAL4       0x04	
#define PIC_ICW1_LEVEL           0x08	
#define PIC_ICW1_INIT            0x10	

#define PIC_ICW4_8086            0x01
#define PIC_ICW4_AUTO            0x02	
#define PIC_ICW4_BUF_SLAVE       0x08	
#define PIC_ICW4_BUF_MASTER      0x0C	
#define PIC_ICW4_SFNM            0x10

#define PIC_1_OFFSET 0x20
#define PIC_2_OFFSET 0x28
#define PIC_2_END PIC_2_OFFSET + 7
#define PIC_1_COMMAND_PORT 0x20
#define PIC_2_COMMAND_PORT 0xA0


void pic_confirm(uint32_t interrupt)
{
	if(interrupt < PIC_1_OFFSET || interrupt > PIC_2_END)
	{
		return;
	}

	if(interrupt < PIC_2_OFFSET)
	{
		outb(PIC_1_COMMAND_PORT, PIC_CONFIRM);
	}
	else
	{
		outb(PIC_2_COMMAND_PORT, PIC_CONFIRM);
	}
}

void pic_remap(uint32_t offset1, uint32_t offset2)
{
	outb(PIC_1_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
	outb(PIC_2_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
	outb(PIC_1_DATA, offset1);
	outb(PIC_2_DATA, offset2);	
	outb(PIC_1_DATA, 4);
	outb(PIC_2_DATA, 2);

	outb(PIC_1_DATA, PIC_ICW4_8086);
	outb(PIC_2_DATA, PIC_ICW4_8086);

	outb(PIC_1_DATA, 0xFD); 
	outb(PIC_2_DATA, 0xFF);

	asm("sti");
}
