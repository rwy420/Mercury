#include <fs/fat32/fat32.h>
#include <memory/heap.h>
#include <common/screen.h>
#include <fs/fat/fat.h>
#include <driver/ata/ata.h>
#include <fs/fat32/fat32_dir.h>

FAT32Volume g_volume;

int fat32_init(BPB* bpb, EBPB_FAT32* ebpb, PartitionTableEntry* partition)
{
	g_volume.bpb = *bpb;
	g_volume.ebpb = *ebpb;
	g_volume.partition = *partition;

	g_volume.fat_start = partition->lba_start + bpb->num_reserved_sectors;
	g_volume.data_start = g_volume.fat_start + (bpb->num_fats * ebpb->num_sectors_per_fat);
	g_volume.root_cluster = ebpb->root_dir_cluster;
	g_volume.sectors_per_cluster = bpb->num_sectors_per_cluster;

	fat_path_to_dir_entry(g_volume.root_cluster, "/BOOT/TEST/KERNEL.ELF");

	return true;
}

uint32_t cluster_to_lba(uint32_t cluster)
{ 
	return g_volume.data_start + (cluster - 2) * g_volume.bpb.num_sectors_per_cluster;
}

uint32_t fat_dir_entry_to_cluster(FATDirectoryEntry* entry)
{
	uint32_t result;

	result = (result & 0xFFFF0000) | entry->first_cluster_lo;
	result = (result & 0x0000FFFF) | ((uint32_t) entry->first_cluster_hi << 16);

	return result;
}
