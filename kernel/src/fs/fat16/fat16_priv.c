#include "fs/fat16/fat16_path.h"
#include "fs/fat16/fat16_rootdir.h"
#include "fs/fat16/fat16_subdir.h"
#include <fs/fat16/fat16.h>
#include <fs/fat16/fat16_priv.h>
#include <core/screen.h>

extern storage_dev_t* dev;
extern FAT16Layout layout;
extern FAT16BPB bpb;

int get_next_cluster(uint16_t* next_cluster, uint16_t cluster)
{
	move_to_fat_region(cluster);
	dev->read(next_cluster, sizeof(cluster));

	return 0;
}

uint32_t move_to_root_directory_region(uint16_t entry_index)
{
	uint32_t pos = layout.start_root_directory_region;
	pos += layout.offset;
	pos += entry_index * 32;
	printf("FAT16: ROOT MOVING TO: ");
	print_hex32(pos);
	printf("\n");
	dev->seek(pos);
	return pos;
}

uint32_t move_to_fat_region(uint16_t cluster)
{
	uint32_t pos = layout.start_fat_region;
	pos += layout.offset;
	pos += cluster * 2;
	printf("FAT16: FAT MOVING TO: ");
	print_hex32(pos);
	printf("\n");
	dev->seek(pos);
	return pos;
}

uint32_t move_to_data_region(uint16_t cluster, uint16_t offset)
{
	print_hex((cluster >> 8) & 0xFF);
	print_hex(cluster & 0xFF);
	printf(" ");
	uint32_t tmp = cluster - 2;

	tmp *= bpb.sectors_per_cluster;
	tmp *= bpb.bytes_per_sector;
	print_hex32(tmp);
	printf(" ");
	uint32_t pos = layout.start_data_region;
	pos += layout.offset;
	pos += tmp;
	pos += offset;
	printf("FAT16: DATA MOVING TO: ");
	print_hex32(pos);
	printf("\n");
	dev->seek(pos);
	return pos;
}

int navigate_to_subdir(EntryHandle* handle, char* entry_name, const char* path)
{
	int ret;
	char subdir_name[13];
	uint16_t index = 0;

	ret = get_subdir(subdir_name, &index, path);
	if(ret < 0) return -1;

	if(to_short_filename(entry_name, subdir_name) < 0) return -1;

	if(open_directory_in_root(handle, entry_name) < 0) return -1;

	while(1)
	{
		ret = get_subdir(subdir_name, &index, path);
		if(to_short_filename(entry_name, &path[index]) < 0) return -1;

		if(ret < 0) break;

		open_directory_in_subdir(handle, entry_name);
	}

	return 0;
}
