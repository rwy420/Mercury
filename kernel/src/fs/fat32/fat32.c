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

	uint32_t root_cluster_lba = cluster_to_lba(volume.root_cluster);
	uint8_t* root_buffer = kmalloc(volume.sectors_per_cluster * 512);

	read28(root_cluster_lba, 0, root_buffer, volume.sectors_per_cluster * 512);

	for(int i = 0; i < 8; i++)
	{
		FATDirectoryEntry* entry = (FATDirectoryEntry*) (root_buffer + i * sizeof(FATDirectoryEntry));
		entry->name[8] = '\0';
		printf((char*) entry->name);
		printf("\n");
	}

	return true;
}

uint32_t cluster_to_lba(uint32_t cluster)
{ 
	return volume.data_start + (cluster - 2) * volume.bpb.num_sectors_per_cluster;
}
