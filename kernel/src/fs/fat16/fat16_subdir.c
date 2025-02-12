#include <fs/fat16/fat16.h>
#include <fs/fat16/fat16_priv.h>
#include <fs/fat16/fat16_subdir.h>
#include <memory/common.h>
#include <core/screen.h>

extern FAT16BPB bpb;
extern storage_dev_t* dev;

static int read_entry_from_subdir(DirEntry* entry, EntryHandle* handle)
{
	printf("READ_ENTRY_SUBDIR\n");
	if(handle->offset == bpb.sectors_per_cluster * bpb.bytes_per_sector)
	{
		uint16_t next_cluster;
		get_next_cluster(&next_cluster, handle->cluster);
		if(next_cluster >= 0xFFF8) return -1;

		move_to_data_region(next_cluster, 0);
		handle->cluster = next_cluster;
		handle->offset = 0;
	}

	move_to_data_region(handle->cluster, handle->offset);
	dev->read(entry, sizeof(DirEntry));
	handle->offset += sizeof(DirEntry);

	return 0;
}

static int find_entry_in_subdir(DirEntry* entry, uint32_t* entry_pos, EntryHandle* handle, char* name)
{
	printf("FIND_ENTRY_SUBDIR\n");
	int ret = -1;
	uint32_t starting_cluster = handle->cluster;

	while(read_entry_from_subdir(entry, handle) == 0)
	{
		if((uint8_t)(entry->name[0]) == AVAILABLE_DIR_ENTRY) continue;

		if(entry->name[0] == 0) break;

		if((entry->attribute & VFAT_DIR_ENTRY) == VFAT_DIR_ENTRY) continue;

		if(memcmp(name, entry->name, sizeof(entry->name)) == 0)
		{
			ret = 0;
			break;
		}
	}

	if(ret == 0 && entry_pos != NULL_PTR)
	{
		*entry_pos = move_to_data_region(handle->cluster, handle->offset);
		*entry_pos -= sizeof(DirEntry);
	}

	handle->cluster = starting_cluster;
	handle->offset = 0;

	return ret;
}

static int open_entry_in_subdir(EntryHandle* handle, char* name, char mode, bool is_file)
{
	printf("OPEN_ENTRY_IN_SUBDIR\n");

	DirEntry entry;
	uint32_t entry_pos;

	if(find_entry_in_subdir(&entry, &entry_pos, handle, name) < 0) return -1;
	if(entry.attribute & VOULME) return -1;
	if(is_file && entry.attribute & SUBDIR) return -1;
	if((entry.attribute & READ_ONLY) && mode != 'r') return -1;

	handle->mode = mode;
	handle->pos_entry = entry_pos;

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

int open_directory_in_subdir(EntryHandle* handle, char* dirname)
{
	printf("OPEN_DIR_SUBDIR");
	return open_entry_in_subdir(handle, dirname, 'r', false);
}

int ls_in_subdir(uint32_t* index, char* name, EntryHandle* handle)
{
	printf("LS_SUBDIR\n");
	DirEntry entry;
	uint32_t entry_index = *index;

	while(entry_index)
	{
		if(read_entry_from_subdir(&entry, handle) < 0) return -1;

		entry_index--;
	}

	printf("WHILE DONE\n");

	if(read_entry_from_subdir(&entry, handle) < 0) return -1;

	if(entry.name[0] == 0) return 0;

	++*index;
	memcpy(name, entry.name, sizeof(entry.name));

	printf("LS DONE\n");

	return 1;
}
