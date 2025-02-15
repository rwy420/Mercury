#include <common/screen.h>
#include <fs/fat16/fat16.h>
#include <fs/fat16/fat16_priv.h>
#include <fs/fat16/fat16_rootdir.h>
#include <memory/common.h>
#include <memory/mem_manager.h>

extern FAT16BPB bpb;
extern storage_dev_t* dev; 

static int find_root_directory_entry(uint16_t* entry_index, char* name)
{
	uint16_t i = 0;

	move_to_root_directory_region(0);
	for(i = 0; i < bpb.root_entry_count; i++)
	{
		DirEntry e;
		dev->read(&e, sizeof(DirEntry));

		if((uint8_t)(e.name[0]) == AVAILABLE_DIR_ENTRY) continue;
		if(e.name[0] == 0) break;
		if((e.attribute & VFAT_DIR_ENTRY) == VFAT_DIR_ENTRY) continue;

		if(memcmp(name, e.name, sizeof(e.name)) == 0)
		{
			*entry_index = i;
			return 0;
		}
	}

	return -1;
}

static int open_entry_in_root(EntryHandle* handle, char* name, char mode, bool is_file)
{
	uint16_t entry_index = 0;
	DirEntry entry;
	
	if(find_root_directory_entry(&entry_index, name) < 0)
		return -1;

	handle->pos_entry = move_to_root_directory_region(entry_index);
	dev->read(&entry, sizeof(DirEntry));

	if(entry.attribute & VOULME) return -1;
	if(is_file && entry.attribute & SUBDIR) return -1;
	if(entry.attribute & READ_ONLY && mode != 'r') return -1;

	handle->mode = mode;

	if(mode == 'a')
	{
		handle->cluster = entry.starting_cluster;
		uint32_t offset = entry.size;
		uint16_t next_cluster;
		get_next_cluster(&next_cluster, handle->cluster);
		
		while(next_cluster < 0xFFF8)
		{
			handle->cluster = next_cluster;
			offset -= bpb.sectors_per_cluster * bpb.bytes_per_sector;
			get_next_cluster(&next_cluster, handle->cluster);
		}

		handle->offset = (uint16_t) offset;
	}
	else
	{
		handle->cluster = entry.starting_cluster;
		handle->offset = 0;
	}

	if(mode == 'r') handle->remaining_bytes = entry.size;
	else handle->remaining_bytes = 0;

	return 0;
}

int open_file_in_root(EntryHandle* handle, char* filename, char mode)
{
	return open_entry_in_root(handle, filename, mode, true);
}

int open_directory_in_root(EntryHandle* handle, char* dirname)
{
	return open_entry_in_root(handle, dirname, 'r', false);
}

int ls_in_root(uint32_t* index, char* filename)
{
	DirEntry entry;

	if(*index == bpb.root_entry_count)
		return 0;
	else if(*index > bpb.root_entry_count)
		return -1;

	move_to_root_directory_region(*index);

	dev->read(&entry, sizeof(DirEntry));
	if(entry.name[0] == 0)
		return 0;

	++*index;
	memcpy(filename, entry.name, 11);

	return 1;
}
