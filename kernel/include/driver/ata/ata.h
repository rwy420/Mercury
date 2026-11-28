#ifndef __MERCURY__DRIVER__ATA__ATA_H
#define __MERCURY__DRIVER__ATA__ATA_H

#define SECTOR_SIZE 512

#include <blkdev.h>
#include <types.h>

typedef struct
{
	uint16_t port_base;
	int master;
} ATAData;

void ata_probe();

int ata_read_blocks(BlkDev* dev, uint32_t lba, void* buffer, size_t size);
int ata_write_blocks(BlkDev* dev, uint32_t lba, void* buffer, size_t size);
int ata_flush(BlkDev* dev);

#endif
