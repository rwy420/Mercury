#ifndef __MERCURY__FS__DISK_H
#define __MERCURY__FS__DISK_H

#include <common/types.h>

typedef struct
{
	uint8_t attributes;
	uint32_t chs_address_start : 24;
	uint8_t type;
	uint32_t chs_address_end : 24;
	uint32_t lba_start;
	uint32_t num_sectors;
} __attribute__((packed)) PartitionTableEntry;

typedef struct
{
	uint8_t boot_code[440];
	uint32_t uid;
	uint16_t reserved;
	PartitionTableEntry partition_table_entries[4];
	uint16_t signature;
} __attribute__((packed)) MBR;

int read_mbr();

#endif
