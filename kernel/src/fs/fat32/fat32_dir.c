#include <fs/fat/fat.h>
#include <fs/fat32/fat32.h>
#include <fs/fat32/fat32_dir.h>
#include <common/screen.h>
#include <memory/common.h>
#include <memory/heap.h>
#include <driver/ata/ata.h>
#include <vesa.h>

extern FAT32Volume g_volume;

void* fat32_lookup(VFSNode* dir, char* name)
{
	FATDirectoryEntry* dir_entry = (FATDirectoryEntry*) dir->fs_object;
	FATDirectoryEntry* result = 0;
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
			result = entry;
			break;
		}
	}

	kfree(cluster_buffer);

	return result;
}
