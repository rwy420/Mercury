#ifndef BOOT__FAT16_H
#define BOOT__FAT16_H

#include "./types.h"

#define FIRST_CLUSTER_INDEX_IN_FAT 3
#define MAX_BYTES_PER_CLUSTER (32768LU)
#define VFAT_DIR_ENTRY 0x0F
#define AVAILABLE_DIR_ENTRY 0xE5

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

enum FileAttribute
{
	READ_ONLY	= 0x01,
	HIDDEN		= 0x02,
	SYSTEM		= 0x04,
	VOULME		= 0x08,
	SUBDIR		= 0x10,
	ARCHIVE		= 0x20
};

bool fat16_read_bpb();
bool fat16_init(uint32_t offset);
bool fat16_read(const char* filepath, char mode, void* buffer, uint32_t count);

int open_file_in_subdir(EntryHandle* handle, char* filename, char mode);
int get_next_cluster(uint16_t* next_cluster, uint16_t cluster);
int read_from_handle(EntryHandle* handle, void* buffer, uint32_t count);
int open_directory_in_subdir(EntryHandle* handle, char* dirname);
int to_short_filename(char* short_filename, const char* long_filename);
bool navigate_to_subdir(EntryHandle* handle, char* entry_name, const char* path);
int get_subdir(char* subdir_name, uint16_t* index, const char* path);
int ls_in_subdir(uint32_t* index, char* name, EntryHandle* handle);

int open_directory_in_root(EntryHandle* handle, char* dirname);
int ls_in_root(uint32_t* index, char* filename);

int fat16_ls(uint32_t* index, char* filename, const char* dir_path);

#endif 
