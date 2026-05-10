#include <memory/heap.h>
#include <common/screen.h>
#include <fs/fat32/fat32.h>
#include <fs/fat/fat.h>
#include <driver/ata/ata.h>

FAT32Volume volume;

int fat32_init(BPB* bpb, EBPB_FAT32* ebpb, PartitionTableEntry* partition)
{
	volume.bpb = *bpb;
	volume.ebpb = *ebpb;
	volume.partition = *partition;

	volume.fat_start = partition->lba_start + bpb->num_reserved_sectors;
	volume.data_start = volume.fat_start + (bpb->num_fats * ebpb->num_sectors_per_fat);
	volume.root_cluster = ebpb->root_dir_cluster;
	volume.sectors_per_cluster = bpb->num_sectors_per_cluster;

	return true;
}

uint32_t cluster_to_lba(uint32_t cluster)
{ 
	return volume.data_start + (cluster - 2) * volume.bpb.num_sectors_per_cluster;
}
