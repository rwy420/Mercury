#include <driver/ata/ata.h>
#include <hardware/port.h>
#include <memory/common.h>
#include <fs/disk.h>
#include <memory/heap.h>
#include <dev.h>
#include <printf.h>

#define DATA_PORT(base) base + 0 // 16 Bit / w Port
#define ERROR_PORT(base) base +  1 // 8 Bit / b Port
#define SECTOR_COUNT_PORT(base) base +  2
#define LBA_LOW_PORT(base) base +  3
#define LBA_MID_PORT(base) base +  4
#define LBA_HI_PORT(base) base +  5
#define DEVICE_PORT(base) base +  6
#define COMMAND_PORT(base) base +  7
#define CONTROL_PORT(base) base +  0x206

BlkDevOps ata_ops = 
{
	ata_read_blocks,
	ata_write_blocks,
	ata_flush
};

static ATAData* init_disk(uint16_t port_base, int master)
{
	ATAData* result = kmalloc(sizeof(ATAData));
	result->port_base = port_base;
	result->master = master;

	return result;
}

static int identify_disk(ATAData* disk)
{
	outb(DEVICE_PORT(disk->port_base), disk->master ? 0xA0 : 0xB0);
	outb(CONTROL_PORT(disk->port_base), 0);

	outb(DEVICE_PORT(disk->port_base), 0xA0);
	uint8_t status = inb(COMMAND_PORT(disk->port_base));
	if(status == 0xFF) return 0;

	outb(DEVICE_PORT(disk->port_base), disk->master ? 0xA0 : 0xB0);
	outb(SECTOR_COUNT_PORT(disk->port_base), 0);
	outb(LBA_LOW_PORT(disk->port_base), 0);
	outb(LBA_MID_PORT(disk->port_base), 0);
	outb(LBA_HI_PORT(disk->port_base), 0);
	outb(COMMAND_PORT(disk->port_base), 0xEC);

	status = inb(COMMAND_PORT(disk->port_base));
	if(status == 0x00) return 0;

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(disk->port_base));

	if(status & 0x01)
	{
		return 0;
	}

	for(uint16_t i = 0; i < 256; i++) inw(DATA_PORT(disk->port_base));

	return 1;
}

void ata_probe()
{
	ATAData* ata;
	ata = init_disk(0x1F0, 1);
	if(identify_disk(ata)) goto register_disk;

	ata = init_disk(0x1F0, 0);
	if(identify_disk(ata)) goto register_disk;

	kfree(ata);
	return;

register_disk:
		Device* dev = kmalloc(sizeof(Device));
		gen_drive_name(dev->name);
		BlkDev* blkdev = kmalloc(sizeof(BlkDev));
		dev->data = blkdev;

		blkdev->block_size = SECTOR_SIZE;
		blkdev->block_count = -1; //TODO
		blkdev->flags = 0;
		blkdev->parent = 0;
		blkdev->ops = &ata_ops;
		blkdev->start_lba = 0;
		blkdev->data = ata;

		blkdev->data = ata;

		read_partition_table(blkdev); //TODO create blkdevs for each partition
		device_register(dev);

		return;
}

static int read28_sector(BlkDev* dev, uint32_t sector, uint8_t* buffer, int length)
{
	if(sector & 0xF0000000) return 0;
	ATAData* data = (ATAData*) dev->data;

	outb(DEVICE_PORT(data->port_base), (data->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	outb(ERROR_PORT(data->port_base), 0);
	outb(SECTOR_COUNT_PORT(data->port_base), 1);

	outb(LBA_LOW_PORT(data->port_base), sector & 0x000000FF);
	outb(LBA_MID_PORT(data->port_base), (sector & 0x0000FF00) >> 8);
	outb(LBA_HI_PORT(data->port_base), (sector & 0x00FF0000) >> 16);
	outb(COMMAND_PORT(data->port_base), 0x20);

	uint8_t status = inb(COMMAND_PORT(data->port_base));

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(data->port_base));

	if(status & 0x1) 
	{
		return 0;
	}

	for(int i = 0; i < length; i+= 2)
	{
		uint16_t r_data = inw(DATA_PORT(data->port_base));

		buffer[i] = r_data & 0x00FF;
		if(i + 1 < length) buffer[i + 1] = (r_data >> 8) & 0x00FF;
	}

	for(int i = length + (length % 2); i < dev->block_size; i+= 2) inw(DATA_PORT(data->port_base));

	return length;
}

static int write28_sector(BlkDev* dev, uint32_t sector, uint8_t* buffer, int length)
{
	if(sector & 0xF0000000) return 0;

	ATAData* data = (ATAData*) dev->data;

	outb(DEVICE_PORT(data->port_base), (data->master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	outb(ERROR_PORT(data->port_base), 0);
	outb(SECTOR_COUNT_PORT(data->port_base), 1);

	outb(LBA_LOW_PORT(data->port_base), sector & 0x000000FF);
	outb(LBA_MID_PORT(data->port_base), (sector & 0x0000FF00) >> 8);
	outb(LBA_HI_PORT(data->port_base), (sector & 0x00FF0000) >> 16);
	outb(COMMAND_PORT(data->port_base), 0x30);

	uint8_t status = inb(COMMAND_PORT(data->port_base));

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(data->port_base));

	for(uint16_t i = 0; i < length; i+= 2)
	{
		uint16_t w_data = buffer[i];
		if(i + 1 < length) w_data |= ((uint16_t) buffer[i + 1]) << 8;

		outw(DATA_PORT(data->port_base), w_data);
	}

	for(int i = length + (length % 2); i < 512; i+= 2) outw(DATA_PORT(data->port_base), 0x00);

	return length;
}

int ata_read_blocks(BlkDev* dev, uint32_t lba, void* buffer, size_t size)
{
	uint8_t temp[dev->block_size];
	uint8_t* result = buffer;

	while(size > 0)
	{
		uint32_t bytes_rep = size;
		if(bytes_rep > dev->block_size) bytes_rep = dev->block_size;

		if(!read28_sector(dev, lba, temp, dev->block_size)) return 0;

		memcpy(result, temp, bytes_rep);

		result += bytes_rep;
		size -= bytes_rep;
		lba++;
	}

	return size;
}

int ata_write_blocks(BlkDev* dev, uint32_t lba, void* buffer, size_t size)
{
	kprintf("ATA_WRITE_BLOCKS IS UNHANDLED\n");
	return 0;
}

int ata_flush(BlkDev* dev)
{
	ATAData* data = (ATAData*) dev->data;
	outb(DEVICE_PORT(data->port_base), data->master ? 0xE0 : 0xF0);
	outb(COMMAND_PORT(data->port_base), 0xE7);

	uint8_t status = inb(COMMAND_PORT(data->port_base));

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) status = inb(COMMAND_PORT(data->port_base));

	if(status & 0x01)
	{
		return 0;
	}

	return 1;
}
