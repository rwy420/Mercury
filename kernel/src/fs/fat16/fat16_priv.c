#include <fs/fat16/fat16_path.h>
#include <fs/fat16/fat16_rootdir.h>
#include <fs/fat16/fat16_subdir.h>
#include <fs/fat16/fat16.h>
#include <fs/fat16/fat16_priv.h>
#include <common/screen.h>

extern storage_dev_t* dev;
extern FAT16Layout layout;
extern FAT16BPB bpb;

int get_next_cluster(uint16_t* next_cluster, uint16_t cluster)
{
	move_to_fat_region(cluster);
	dev->read(next_cluster, sizeof(cluster));

	return 0;
}

int read_from_handle(EntryHandle* handle, void* buffer, uint32_t count)
{
	uint32_t bytes_read_count = 0;
	uint8_t* bytes = (uint8_t*) buffer;

	if(handle->remaining_bytes == 0) return 0;
	if(handle->cluster == 0) return 0;

	move_to_data_region(handle->cluster, handle->offset);

	while(count > 0)
	{
		uint32_t chunk_length = count, bytes_remaining_in_cluster = 0;

		if(handle->remaining_bytes == 0) return bytes_read_count;

		bytes_remaining_in_cluster = bpb.sectors_per_cluster * bpb.bytes_per_sector - handle->offset;

		if(chunk_length > bytes_remaining_in_cluster) chunk_length = bytes_remaining_in_cluster;
		if(chunk_length > handle->remaining_bytes) chunk_length = handle->remaining_bytes;

		dev->read(&bytes[bytes_read_count], chunk_length);

		handle->remaining_bytes -= chunk_length;
		handle->offset += chunk_length;

		if(handle->offset == bpb.sectors_per_cluster * bpb.bytes_per_sector)
		{
			handle->offset = 0;

			if(handle->remaining_bytes != 0)
			{
				uint16_t next_cluster;
				if(get_next_cluster(&next_cluster, handle->cluster) < 0) return -1;

				handle->cluster = next_cluster;
				move_to_data_region(handle->cluster, handle->offset);
			}
		}

		count -= chunk_length;
		bytes_read_count += chunk_length;
	}

	return bytes_read_count;
}

uint32_t move_to_root_directory_region(uint16_t entry_index)
{
	uint32_t pos = layout.start_root_directory_region;
	pos += layout.offset;
	pos += entry_index * 32;
	//printf("FAT16: ROOT MOVING TO: ");
	//print_hex32(pos);
	//printf("\n");
	dev->seek(pos);
	return pos;
}

uint32_t move_to_fat_region(uint16_t cluster)
{
	uint32_t pos = layout.start_fat_region;
	pos += layout.offset;
	pos += cluster * 2;
	//printf("FAT16: FAT MOVING TO: ");
	//print_hex32(pos);
	//printf("\n");
	dev->seek(pos);
	return pos;
}

uint32_t move_to_data_region(uint16_t cluster, uint16_t offset)
{
	uint32_t tmp = cluster - 2;

	tmp *= bpb.sectors_per_cluster;
	tmp *= bpb.bytes_per_sector;
	uint32_t pos = layout.start_data_region;
	pos += layout.offset;
	pos += tmp;
	pos += offset;
	//printf("FAT16: DATA MOVING TO: ");
	//print_hex32(pos);
	//printf("\n");
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
