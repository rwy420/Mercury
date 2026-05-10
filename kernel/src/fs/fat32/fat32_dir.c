#include "fs/fat/fat.h"
#include <fs/fat32/fat32.h>
#include <fs/fat32/fat32_dir.h>
#include <common/screen.h>
#include <memory/common.h>
#include <memory/heap.h>
#include <driver/ata/ata.h>
#include <vesa.h>

extern FAT32Volume g_volume;

FATDirectoryEntry* fat_path_to_dir_entry(uint32_t root_cluster, char* path)
{
	uint8_t* cluster_buffer = kmalloc(g_volume.sectors_per_cluster * 512);
	uint32_t current_cluster = root_cluster;
	FATDirectoryEntry* result;

	uint32_t path_idx = 0;
	uint8_t current_sub[12];
	uint8_t current_sub_idx = 0;
	int done = 0;
	int found = 0;

	memset(current_sub, 0, 11);
	current_sub[11] = '\0';

	while(!done)
	{
		char current = path[path_idx];
		done = path[path_idx + 1] == '\0';

		if(current == '/' || done)
		{
			if(done) current_sub[current_sub_idx++] = current;

			read28(cluster_to_lba(current_cluster), 0, cluster_buffer, g_volume.sectors_per_cluster * 512);

			for(int i = 0; i < (g_volume.sectors_per_cluster * 512) / sizeof(FATDirectoryEntry); i++)
			{
				FATDirectoryEntry* entry = (FATDirectoryEntry*) (cluster_buffer + i * sizeof(FATDirectoryEntry));
				if(entry->name[0] == 0x00) break;
				if(entry->name[0] == 0xE5) break;
				if(entry->attributes == 0x0F) continue; //TODO: LFN
				
				char current_sub_83[12];
				current_sub_83[11] = '\0';
				
				string_to_83_name(current_sub_83, current_sub);
	
				if(!memcmp(current_sub_83, entry->name, 11))
				{
					current_cluster = fat_dir_entry_to_cluster(entry);
					/*printf("MATCH: '");
					printf(current_sub_83);
					printf("':'");
					printf(entry->name);
					printf("'\n\n");*/

					if(done)
					{
						found = 1;
						result = entry;
					}
				}
			}

			current_sub_idx = 0;
			memset(current_sub, 0, 11);
		}
		else
		{
			current_sub[current_sub_idx++] = current;
		}

		path_idx++;
	}

	kfree(cluster_buffer);

	if(!found)
	{
		printf_color("<FAT32> Could not find '", COLOR_RED, COLOR_BLACK);
		printf_color(path, COLOR_RED, COLOR_BLACK);
		printf_color("'\n", COLOR_RED, COLOR_BLACK);
		return NULL_PTR;
	}

	return result;
}
