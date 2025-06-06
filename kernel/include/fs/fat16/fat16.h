#ifndef __MERCURY__FS__FAT16__FAT16_H
#define __MERCURY__FS__FAT16__FAT16_H

#include <common/types.h>

typedef enum
{
	INAVLID_JUMP_INSTRUCTION,
	INVALID_BYTES_PER_SECTOR,
	INVALID_SECTORS_PER_CLUSTER,
	INVALID_RESERVED_SECTOR_COUNT,
	INVALID_ROOT_ENTRY_COUNT,
	INVALID_SECTOR_COUNT,
	INVALID_FAT_TYPE
} FAT_ERROR;

typedef struct
{
	int (*read)(void* buffer, uint32_t length);
	int (*read_byte)(void* buffer);
	int (*write)(const void* buffer, uint32_t length);
	int (*seek)(uint32_t offset);
} storage_dev_t;

int fat16_read_bpb();
int fat16_init(storage_dev_t* dev, uint32_t offset);
int fat16_open(const char* filepath, char mode);
int fat16_size(const char* filepath);
int fat16_close(uint8_t handle);
int fat16_read(uint8_t handle, void* buffer, uint32_t count);
int fat16_ls(uint32_t* index, char* filename, const char* dir_path);

int _read(void* buffer, uint32_t length);
int _read_byte(void* buffer);
int _write(const void* buffer, uint32_t length);
int _seek(uint32_t offset);
#endif
