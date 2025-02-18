#ifndef __MERCURY__FS__FAT16__FAT16_PRIV_H
#define __MERCURY__FS__FAT16__FAT16_PRIV_H

#define FIRST_CLUSTER_INDEX_IN_FAT 3
#define MAX_BYTES_PER_CLUSTER (32768LU)
#define VFAT_DIR_ENTRY 0x0F
#define AVAILABLE_DIR_ENTRY 0xE5

#include <common/types.h>

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

typedef struct
{
	char name[11];
	uint8_t attribute;
	uint8_t reserved[10];
	uint8_t time[2];
	uint8_t date[2];
	uint16_t starting_cluster;
	uint32_t size;
} __attribute__((packed)) DirEntry;

enum FAT16_FILE_ATTRIBUTE
{
	READ_ONLY	= 0x01,
	HIDDEN		= 0x02,
	SYSTEM		= 0x04,
	VOULME		= 0x08,
	SUBDIR		= 0x10,
	ARCHIVE		= 0x20
};

int get_next_cluster(uint16_t* next_cluster, uint16_t cluster);
int read_from_handle(EntryHandle* handle, void* buffer, uint32_t count);
uint32_t move_to_root_directory_region(uint16_t entry_index);
uint32_t move_to_fat_region(uint16_t cluster);
uint32_t move_to_data_region(uint16_t cluster, uint16_t offset);
int navigate_to_subdir(EntryHandle* handle, char* entry_name, const char* path);

#endif
