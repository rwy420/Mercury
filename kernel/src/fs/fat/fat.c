#include <fs/fat32/fat32.h>
#include <fs/fat/fat.h>
#include <memory/common.h>

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

int string_to_83_name(char* name_83, char* path_name)
{
	char result[12];
	memset(result, 0, 12);

	for(int i = 0, path_idx = 0; i < 11; i++, path_idx++)
	{
		char current = path_name[path_idx];

		if(current == '.')
		{
			for(int j = i; j < 8; j++) result[j] = ' ';
			
			i = 7;

			continue;
		}
		else if(current == '\0')
		{
			for(int j = i; j < 11; j++) result[j] = ' ';

			break;
		}
		
		result[i] = current;
	}

	memcpy(name_83, result, 11);

	return true;
}
