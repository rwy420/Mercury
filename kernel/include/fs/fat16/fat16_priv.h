#ifndef __QS__FS__FAT16__FAT16_PRIV_H
#define __QS__FS__FAT16__FAT16_PRIV_H

#include <core/types.h>

typedef struct
{
	uint32_t offset;
	uint32_t start_fat_region;
	uint32_t start_root_directory_region;
	uint32_t start_data_region;
	uint32_t data_cluster_count;
} FAT16Layout;

typedef struct
{
	char oem_name[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sector_count;
	uint8_t num_fats;
	uint16_t root_entry_count;
	uint32_t sector_count;
	uint16_t fat_size;
	uint32_t volume_id;
	char label[11];
	char fs_type[8];
} FAT16BPB;

typedef struct
{
	char mode;
	uint32_t pos_entry;
	uint16_t cluster;
	uint16_t offset;
	uint32_t remaining_bytes;
} EntryHandle;

#endif
