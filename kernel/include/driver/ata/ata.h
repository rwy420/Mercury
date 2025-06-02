#ifndef __MERCURY__DRIVER__ATA__ATA_H
#define __MERCURY__DRIVER__ATA__ATA_H

#include <common/types.h>

typedef struct
{
	uint16_t port_base;
	int master;
} Disk;

Disk init_disk(uint16_t port_base, int master);
int identify_disk(Disk* disk);

int read28_disk(Disk* disk, uint32_t sector, uint8_t* buffer, int length);
int write28_disk(Disk* disk, uint32_t sector, uint8_t* data, int length);
int flush_disk(Disk* disk);

int read28(uint32_t sector, uint8_t* buffer, int length);
int write28(uint32_t sector, uint8_t* data, int length);
int flush();

#endif
