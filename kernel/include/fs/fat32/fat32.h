#ifndef __MERCURY__FS__FAT32__FAT32_H
#define __MERCURY__FS__FAT32__FAT32_H

#include <fs/disk.h>
#include <fs/fat/bpb.h>

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

int fat32_init(BPB* bpb, EBPB_FAT32* ebpb, PartitionTableEntry* partition);

uint32_t cluster_to_lba(uint32_t cluster);

#endif
