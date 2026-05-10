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

#endif
