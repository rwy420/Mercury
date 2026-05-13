#ifndef __MERCURY__FS__FAT32__FAT32_H
#define __MERCURY__FS__FAT32__FAT32_H

#include <fs/fat/fat.h>
#include <fs/disk.h>
#include <fs/fat/bpb.h>
#include <fd.h>

typedef struct
{
	BPB bpb;
	EBPB_FAT32 ebpb;
	PartitionTableEntry partition;
	uint32_t fat_start;
	uint32_t data_start;
	uint32_t root_cluster;
	uint32_t sectors_per_cluster;
} FAT32Volume;

typedef struct
{
	FATDirectoryEntry* entry;
} FAT32File;

int fat32_init(BPB* bpb, EBPB_FAT32* ebpb, PartitionTableEntry* partition);
uint32_t fat_get_next_cluster(uint32_t cluster);

int fat32_open(FileDescriptor* fd, char* path);
int fat32_read(void* file_object, void* buffer, uint32_t offset, size_t length);
int fat32_write(void* file_object, void* buffer, uint32_t offset, size_t length);
int fat32_close(void* file_object);

#endif
