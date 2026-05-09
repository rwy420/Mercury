#ifndef __MERCURY__FS__FAT__BPB_H
#define __MERCURY__FS__FAT__BPB_H

#include <common/types.h>

typedef struct
{
	uint8_t jmp_short[3];
	uint64_t oem;
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t num_reserved_sectors;
	uint8_t num_fats;
	uint16_t num_root_dir_entries;
	uint16_t num_total_sectors;
	uint8_t media_descriptor;
	uint16_t num_sectors_per_fat;
	uint16_t num_sectors_per_track;
	uint16_t num_heads;
	uint32_t num_hidden_sectors;
	uint32_t num_large_sectors;
} __attribute__((packed)) BPB;

int fat_read_bpb(uint32_t sector);

#endif
