#include <fs/fat32/fat32.h>
#include <memory/heap.h>
#include <common/screen.h>
#include <fs/fat/fat.h>
#include <driver/ata/ata.h>
#include <fs/fat32/fat32_dir.h>
#include <memory/common.h>
#include <fs/vfs/vfs.h>

VFSNodeOps g_fat32_ops = 
{
	.lookup = fat32_lookup,
	.open = fat32_open,
	.read = fat32_read,
	.write = fat32_write,
	.close = fat32_close
};

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

	return true;
}

uint32_t fat_get_next_cluster(uint32_t cluster)
{
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = g_volume.fat_start + (fat_offset / 512);
    uint32_t entry_offset = fat_offset % 512;
    
    uint8_t sector_buffer[512];
    read28(fat_sector, 0, sector_buffer, 512);
    
    uint32_t next = *(uint32_t*)(sector_buffer + entry_offset) & 0x0FFFFFFF;
    return next;
}

int fat32_open(VFSNode* node)
{
	return 0;
}

int fat32_read(VFSNode* node, void* buffer, uint32_t offset, size_t length)
{
	FATDirectoryEntry* dir_entry = ((FAT32File*) node->fs_object)->entry;
	uint32_t dir_entry_sector_size = (dir_entry->size + 511) / 512;
	uint32_t current_cluster = fat_dir_entry_to_cluster(dir_entry);
	uint32_t sector_offset = offset / 512;
	uint32_t sector_offset_start = offset % 512;
	uint32_t bytes_read = 0;
	uint32_t sectors_current_cluster = 0;

	uint8_t sector_buffer[512];

	for(uint32_t i = 0; i < dir_entry_sector_size; i++)
	{
		uint32_t bytes_to_read = 512 - sector_offset_start;
		if(bytes_to_read > length) bytes_to_read = length;

		uint32_t sector = cluster_to_lba(current_cluster) + sectors_current_cluster;
		read28(sector, sector_offset_start, sector_buffer, bytes_to_read);
		memcpy(buffer + bytes_read, sector_buffer, bytes_to_read);
		memset(sector_buffer, 0, 512);

		if(sectors_current_cluster >= g_volume.sectors_per_cluster)
		{
			current_cluster = fat_get_next_cluster(current_cluster);
			sectors_current_cluster = 0;
		}

		sector_offset_start = 0;
		bytes_read += bytes_to_read;
		length -= bytes_to_read;
		sectors_current_cluster++;	
	}

	return 0;
}

int fat32_write(VFSNode* node, void* buffer, uint32_t offset, size_t length)
{

}

int fat32_close(VFSNode* node)
{
	return 0;
}
