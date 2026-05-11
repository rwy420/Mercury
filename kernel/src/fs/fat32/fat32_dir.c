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

	int found = 0;
	char* start = path;

	if(*start == '/') start++;

	while(*start)
	{
		char* end = start;

		while(*end && *end != '/')
		{
			end++;
		}

		char saved = *end;
		*end = '\0';

			read28(cluster_to_lba(current_cluster), 0, cluster_buffer, g_volume.sectors_per_cluster * 512);

			for(int i = 0; i < (g_volume.sectors_per_cluster * 512) / sizeof(FATDirectoryEntry); i++)
			{
				result = 0;

				FATDirectoryEntry* entry = (FATDirectoryEntry*) (cluster_buffer + i * sizeof(FATDirectoryEntry));
				if(entry->name[0] == 0x00) break;
				if(entry->name[0] == 0xE5) break;
				if(entry->attributes == 0x0F) continue; //TODO: LFN
				
				char current_sub_83[12];
				current_sub_83[11] = '\0';
				
				string_to_83_name(current_sub_83, start);
	
				if(!memcmp(current_sub_83, entry->name, 11))
				{
					current_cluster = fat_dir_entry_to_cluster(entry);
					result = entry;
					break;
				}
			}

		*end = saved;

		start = end;

		while(*start == '/') start++;
	}

	kfree(cluster_buffer);

	if(result == 0)
	{
		printf_color("<FAT32> Could not find '", COLOR_RED, COLOR_BLACK);
		printf_color(path, COLOR_RED, COLOR_BLACK);
		printf_color("'\n", COLOR_RED, COLOR_BLACK);
		return NULL_PTR;
	}

	return result;
}
