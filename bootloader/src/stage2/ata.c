#include "./ata.h"
#include "./io.h"
#include "./common.h"

#define DATA_PORT(base) base + 0 // 16 Bit / w Port
#define ERROR_PORT(base) base +  1 // 8 Bit / b Port
#define SECTOR_COUNT_PORT(base) base +  2
#define LBA_LOW_PORT(base) base +  3
#define LBA_MID_PORT(base) base +  4
#define LBA_HI_PORT(base) base +  5
#define DEVICE_PORT(base) base +  6
#define COMMAND_PORT(base) base +  7
#define CONTROL_PORT(base) base +  0x206

uint32_t disk_pointer;
extern Disk* default_disk;

Disk init_disk(uint16_t port_base, bool master)
{
	Disk result;
	result.port_base = port_base;
	result.master = master;

	return result;
}

bool identify_disk(Disk* disk)
{
	disk_pointer = 0;

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

bool read28(Disk* disk, uint32_t sector, uint8_t* buffer)
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

	for(int i = 0; i < 512; i+= 2)
	{
		uint16_t r_data = inw(DATA_PORT(disk->port_base));

		buffer[i] = r_data & 0x00FF;
		buffer[i + 1] = (r_data >> 8) & 0x00FF;
	}

	return true;
}

int read(void* buffer, uint32_t length)
{
    uint8_t sector_buffer[512];
    uint8_t* buf = (uint8_t*) buffer;

    uint32_t sector = disk_pointer / 512;     // Starting sector
	sector += 100;
    uint32_t offset = disk_pointer % 512;     // Byte offset in that sector

    uint32_t remaining = length;

    // Handle first partial sector
    if (offset != 0) {
        read28(default_disk, sector, sector_buffer);
        uint32_t to_copy = (remaining < (512 - offset)) ? remaining : (512 - offset);
        memcpy(buf, sector_buffer + offset, to_copy);
        buf += to_copy;
        remaining -= to_copy;
        sector++;
        disk_pointer += to_copy;
    }

    // Read full sectors
    while (remaining >= 512) {
        read28(default_disk, sector, buf);
        buf += 512;
        remaining -= 512;
        sector++;
        disk_pointer += 512;
    }

    // Handle last partial sector
    if (remaining > 0) {
        read28(default_disk, sector, sector_buffer);
        memcpy(buf, sector_buffer, remaining);
        disk_pointer += remaining;
    }
	
	return 0;
}

int read_byte(void* buffer)
{
	return read(buffer, 1);
}

int seek(uint32_t offset)
{
	disk_pointer = offset;
	return 0;
}
