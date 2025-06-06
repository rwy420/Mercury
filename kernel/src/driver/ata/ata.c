#include <driver/ata/ata.h>
#include <hardware/port.h>
#include <common/screen.h>

#define DATA_PORT(base) base + 0 // 16 Bit / w Port
#define ERROR_PORT(base) base +  1 // 8 Bit / b Port
#define SECTOR_COUNT_PORT(base) base +  2
#define LBA_LOW_PORT(base) base +  3
#define LBA_MID_PORT(base) base +  4
#define LBA_HI_PORT(base) base +  5
#define DEVICE_PORT(base) base +  6
#define COMMAND_PORT(base) base +  7
#define CONTROL_PORT(base) base +  0x206

Disk* g_default_disk;
uint16_t sector_bytes = 512;

Disk init_disk(uint16_t port_base, int master)
{
	Disk result;
	result.port_base = port_base;
	result.master = master;

	return result;
}

int identify_disk(Disk* disk)
{
	outb(DEVICE_PORT(disk->port_base), disk->master ? 0xA0 : 0xB0);
	outb(CONTROL_PORT(disk->port_base), 0);

	outb(DEVICE_PORT(disk->port_base), 0xA0);
	uint8_t status = inb(COMMAND_PORT(disk->port_base));
	if(status == 0xFF) return false;

	outb(DEVICE_PORT(disk->port_base), disk->master ? 0xA0 : 0xB0);
	outb(SECTOR_COUNT_PORT(disk->port_base), 0);
	outb(LBA_LOW_PORT(disk->port_base), 0);
	outb(LBA_MID_PORT(disk->port_base), 0);
	outb(LBA_HI_PORT(disk->port_base), 0);
	outb(COMMAND_PORT(disk->port_base), 0xEC);

	status = inb(COMMAND_PORT(disk->port_base));
	if(status == 0x00) return false;

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(disk->port_base));

	if(status & 0x01)
	{
		return false;
	}

	for(uint16_t i = 0; i < 256; i++) inw(DATA_PORT(disk->port_base));

	return true;
}

int read28_disk(Disk* disk, uint32_t sector, uint8_t* buffer, int length)
{
	if(sector & 0xF0000000) return false;

	outb(DEVICE_PORT(disk->port_base), (disk->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	outb(ERROR_PORT(disk->port_base), 0);
	outb(SECTOR_COUNT_PORT(disk->port_base), 1);

	outb(LBA_LOW_PORT(disk->port_base), sector & 0x000000FF);
	outb(LBA_MID_PORT(disk->port_base), (sector & 0x0000FF00) >> 8);
	outb(LBA_HI_PORT(disk->port_base), (sector & 0x00FF0000) >> 16);
	outb(COMMAND_PORT(disk->port_base), 0x20);

	uint8_t status = inb(COMMAND_PORT(disk->port_base));

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(disk->port_base));

	if(status & 0x1) 
	{
		return false;
	}

	for(int i = 0; i < length; i+= 2)
	{
		uint16_t r_data = inw(DATA_PORT(disk->port_base));

		buffer[i] = r_data & 0x00FF;
		if(i + 1 < length) buffer[i + 1] = (r_data >> 8) & 0x00FF;
	}

	for(int i = length + (length % 2); i < sector_bytes; i+= 2) inw(DATA_PORT(disk->port_base));

	return true;
}

int write28_disk(Disk* disk, uint32_t sector, uint8_t* data, int length)
{
	if(sector & 0xF0000000) 
	{
		return false;
	}

	outb(DEVICE_PORT(disk->port_base), (disk->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	outb(ERROR_PORT(disk->port_base), 0);
	outb(SECTOR_COUNT_PORT(disk->port_base), 1);

	outb(LBA_LOW_PORT(disk->port_base), sector & 0x000000FF);
	outb(LBA_MID_PORT(disk->port_base), (sector & 0x0000FF00) >> 8);
	outb(LBA_HI_PORT(disk->port_base), (sector & 0x00FF0000) >> 16);
	outb(COMMAND_PORT(disk->port_base), 0x30);

	uint8_t status = inb(COMMAND_PORT(disk->port_base));

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(disk->port_base));

	for(uint16_t i = 0; i < length; i+= 2)
	{
		uint16_t w_data = data[i];
		if(i + 1 < length) w_data |= ((uint16_t) data[i + 1]) << 8;

		outw(DATA_PORT(disk->port_base), w_data);
	}

	for(int i = length + (length % 2); i < 512; i+= 2) outw(DATA_PORT(disk->port_base), 0x00);

	return true;
}

int flush_disk(Disk* disk)
{
	outb(DEVICE_PORT(disk->port_base), disk->master ? 0xE0 : 0xF0);
	outb(COMMAND_PORT(disk->port_base), 0xE7);

	uint8_t status = inb(COMMAND_PORT(disk->port_base));

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(disk->port_base));

	if(status & 0x01)
	{
		return false;
	}

	return true;
}

int read28(uint32_t sector, uint8_t* buffer, int length)
{
	return read28_disk(g_default_disk, sector, buffer, length);
}
int write28(uint32_t sector, uint8_t* data, int length)
{
	return write28_disk(g_default_disk, sector, data, length);
}

int flush()
{
	return flush_disk(g_default_disk);
}
