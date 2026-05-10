#include <fs/fat32/fat32.h>
#include <fs/fat/fat.h>

extern FAT32Volume g_volume;

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
