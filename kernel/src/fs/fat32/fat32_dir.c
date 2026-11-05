#include "fs/vfs/vfs.h"
#include <fs/fat/fat.h>
#include <fs/fat32/fat32.h>
#include <fs/fat32/fat32_dir.h>
#include <common/screen.h>
#include <memory/common.h>
#include <memory/heap.h>
#include <driver/ata/ata.h>
#include <vesa.h>

extern FAT32Volume g_volume;
extern VFSNodeOps g_fat32_ops;

VFSNode* fat32_lookup(VFSNode* dir, char* name)
{
	FATDirectoryEntry* dir_entry = (FATDirectoryEntry*) dir->fs_object;
	VFSNode* result = kmalloc(sizeof(VFSNode));
	FATDirectoryEntry* result_entry = kmalloc(sizeof(FATDirectoryEntry));
	uint8_t* cluster_buffer = kmalloc(g_volume.sectors_per_cluster * 512);

	read28(cluster_to_lba(fat_dir_entry_to_cluster(dir_entry)), 0, cluster_buffer, g_volume.sectors_per_cluster * 512);

	for(int i = 0; i < (g_volume.sectors_per_cluster * 512) / sizeof(FATDirectoryEntry); i++)
	{
		FATDirectoryEntry* entry = (FATDirectoryEntry*) (cluster_buffer + i * sizeof(FATDirectoryEntry));
		if(entry->name[0] == 0x00) break;
		if(entry->name[0] == 0xE5) break;
		if(entry->attributes == 0x0F) continue; //TODO: LFN

		char current_entry_83[12];
		current_entry_83[11] = '\0';
				
		string_to_83_name(current_entry_83, name);

		if(!memcmp(current_entry_83, entry->name, 11))
		{
			memcpy(result_entry, entry, sizeof(FATDirectoryEntry));
			result->fs_object = result_entry;
			result->ops = &g_fat32_ops;
			result->mounted = 0;
			result->type = 0;
			break;
		}
	}

	kfree(cluster_buffer);

	return result;
}
