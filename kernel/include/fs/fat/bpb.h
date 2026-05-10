#ifndef __MERCURY__FS__FAT__BPB_H
#define __MERCURY__FS__FAT__BPB_H

#include <common/types.h>
#include <fs/disk.h>

typedef struct
{
	uint8_t jmp_short[3];
	uint64_t oem;
	uint16_t bytes_per_sector;
	uint8_t num_sectors_per_cluster;
	uint16_t num_reserved_sectors;
	uint8_t num_fats;
	uint16_t num_root_dir_entries;
	uint16_t num_total_sectors;
	uint8_t media_descriptor;
	uint16_t num_sectors_per_fat;
	uint16_t num_sectors_per_track;
	uint16_t num_heads;
	uint32_t num_hidden_sectors;
	uint32_t large_num_sectors;
} __attribute__((packed)) BPB;

typedef struct
{
	uint32_t num_sectors_per_fat;
	uint16_t flags;
	uint16_t fat_version;
	uint32_t root_dir_cluster;
	uint16_t fsinfo_sector;
	uint16_t backup_boot_sector;
	uint8_t reserved[12];
	uint8_t drive;
	uint8_t wnt_flags;
	uint8_t signature;
	uint32_t volume_id;
	uint8_t volume_label[11];
	uint8_t system_identifier[8];
	uint8_t boot_code[420];
	uint16_t boot_signature;
} __attribute__((packed)) EBPB_FAT32;

int fat_read_bpb(PartitionTableEntry* partition);

#endif
