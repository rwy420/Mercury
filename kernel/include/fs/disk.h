#ifndef __MERCURY__FS__DISK_H
#define __MERCURY__FS__DISK_H

#include <common/types.h>

typedef struct
{
	uint8_t attributes;
	uint8_t chs_address_start[3];
	uint8_t type;
	uint8_t chs_address_end[3];
	uint8_t lba_start[4];
	uint8_t num_sectors[4];
} __attribute__((packed)) PartitionTableEntry;

typedef struct
{
	uint8_t bootstrap[440];
	uint32_t uid;
	uint16_t reserved;
	PartitionTableEntry partition_table_entries[4];
	uint16_t signature;
} __attribute__((packed)) MBR;

int read_mbr();

#endif
