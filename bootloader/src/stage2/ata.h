#ifndef BOOT__ATA_H
#define BOOT__ATA_H

#include "./types.h"

typedef struct
{
	uint16_t port_base;
	bool master;
} Disk;

Disk init_disk(uint16_t port_base, bool master);
bool identify_disk(Disk* disk);
bool read28(Disk* disk, uint32_t sector, uint8_t* buffer);

int read(void* buffer, uint32_t length);
int read_byte(void* buffer);
int seek(uint32_t offset);

#endif
