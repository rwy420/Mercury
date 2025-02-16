#ifndef __MERCURY__DRIVER__ATA__ATA_H
#define __MERCURY__DRIVER__ATA__ATA_H

#include <common/types.h>

typedef struct
{
	uint16_t port_base;
	bool master;
} __attribute__((packed)) Disk;

Disk init_disk(uint16_t port_base, bool master);
bool identify_disk(Disk* disk);

void set_default_disk(Disk* disk);
Disk* get_default_disk();

bool read28_disk(Disk* disk, uint32_t sector, uint8_t* buffer, int length);
bool write28_disk(Disk* disk, uint32_t sector, uint8_t* data, int length);
bool flush_disk(Disk* disk);

bool read28(uint32_t sector, uint8_t* buffer, int length);
bool write28(uint32_t sector, uint8_t* data, int length);
bool flush();

#endif
