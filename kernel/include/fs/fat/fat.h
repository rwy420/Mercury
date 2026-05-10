#ifndef __MERCURY__FS__FAT__FAT_H
#define __MERCURY__FS__FAT__FAT_H

#include <common/types.h>

typedef struct
{
	uint8_t name[11];
	uint8_t attributes;
	uint8_t wnt_reserved;
	uint8_t creation_time_sec;
	uint16_t creation_time;
	uint16_t creation_date;
	uint16_t last_accessed_date;
	uint16_t first_cluster_hi;
	uint16_t last_mod_time;
	uint16_t last_mod_date;
	uint16_t first_cluster_lo;
	uint32_t size;
} __attribute__((packed)) FATDirectoryEntry;

typedef struct
{
	uint8_t order;
	uint16_t lo5_w[5];
	uint8_t attribute;
	uint8_t type;
	uint8_t checksum;
	uint16_t mi6_w[6];
	uint16_t zero;
	uint16_t hi2_w[2];
} __attribute__((packed)) FATDirectoryEntryLFN;

uint32_t cluster_to_lba(uint32_t cluster);
uint32_t fat_dir_entry_to_cluster(FATDirectoryEntry* entry);
int string_to_83_name(char* name_83, char* path_name);

#endif
