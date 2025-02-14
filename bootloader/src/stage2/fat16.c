#include "./fat16.h"
#include "./ata.h"
#include "./common.h"
#include "./screen.h"

FAT16BPB bpb;
FAT16Layout layout;

bool fat16_read_bpb()
{
	uint8_t data;
	memset(&bpb, 0, sizeof(FAT16BPB));
	memset(&layout, 0, sizeof(FAT16Layout));
	seek(layout.offset);

#ifdef FAT_DEBUG 
	printf("FAT16: #####   BPB   #####\n");
#endif
	read_byte(&data);
	if(data == 0xEB)
	{
		read_byte(&data);
		read_byte(&data);
		if(data != 0x90)
		{
			return false;
		}
	}
	else if(data == 0xE9)
	{
		read_byte(&data);
		read_byte(&data);
	}
	else
	{
		return false;
	}

	read(&bpb.oem_name, 8);
#ifdef FAT_DEBUG
	printf("FAT16: OEM NAME: ");
	printf(bpb.oem_name);
	printf("\n");
#endif

	read(&bpb.bytes_per_sector, 2);
#ifdef FAT_DEBUG
	printf("FAT16: BYTES PER SECTOR: ");
	print_hex((bpb.bytes_per_sector >> 8) & 0xFF);
	print_hex(bpb.bytes_per_sector & 0xFF);
	printf("\n");
#endif

	if(bpb.bytes_per_sector != 512) return false;

	read(&bpb.sectors_per_cluster, 1);
#ifdef FAT_DEBUG
	printf("FAT16: SECTORS PER CLUSTER ");
	print_hex(bpb.sectors_per_cluster);
	printf("\n");
#endif

	if(bpb.sectors_per_cluster != 1 && bpb.sectors_per_cluster != 2 && bpb.sectors_per_cluster != 4
			&& bpb.sectors_per_cluster != 8 && bpb.sectors_per_cluster != 16 && bpb.sectors_per_cluster != 32
			&& bpb.sectors_per_cluster != 64 && bpb.sectors_per_cluster != 128)
		return false;

	read(&bpb.reserved_sector_count, 2);
#ifdef FAT_DEBUG
	printf("FAT16: RESERVED SECTOR COUNT: ");
	print_hex((bpb.reserved_sector_count >> 8) & 0xFF);
	print_hex(bpb.reserved_sector_count & 0xFF);
	printf("\n");
#endif

	read(&bpb.num_fats, 1);
#ifdef FAT_DEBUG
	printf("FAT16: NUM FATS: ");
	print_hex(bpb.num_fats);
	printf("\n");
#endif

	read(&bpb.root_entry_count, 2);
#ifdef FAT_DEBUG
	printf("FAT16: ROOT ENTRY COUNT: ");
	print_hex((bpb.root_entry_count >> 8) & 0xFF);
	print_hex(bpb.root_entry_count & 0xFF);
	printf("\n");
#endif

	read(&bpb.sector_count, 2);
	
	read_byte(&data);

	read(&bpb.fat_size, 2);
#ifdef FAT_DEBUG
	printf("FAT16: FAT SIZE: ");
	print_hex((bpb.fat_size >> 8) & 0xFF);
	print_hex(bpb.fat_size & 0xFF);
	printf("\n");
#endif

	read_byte(&data);
	read_byte(&data);

	read_byte(&data);
	read_byte(&data);


	read_byte(&data);
	read_byte(&data);
	read_byte(&data);
	read_byte(&data);

	uint32_t sector_count_32;
	read(&sector_count_32, 4);
	if((bpb.sector_count != 0 && sector_count_32 != 0) || (bpb.sector_count == 0 && sector_count_32 == 0))
		return false;

	read_byte(&data);
	read_byte(&data);

	read_byte(&data);
	if(data == 0x29)
	{
		read(&bpb.volume_id, 4);
#ifdef FAT_DEBUG
		printf("FAT16: VOLUME ID: ");
		print_hex32(bpb.volume_id);
		printf("\n");
#endif

		read(&bpb.label, 11);
#ifdef FAT_DEBUG
		printf("FAT16: LABEL: ");
		printf(bpb.label);
		printf("\n");
#endif

		read(&bpb.fs_type, 8);
#ifdef FAT_DEBUG
		printf("FAT16: FS TYPE: ");
		printf(bpb.fs_type);
		printf("\n");
#endif
	}

	return true;
}

bool fat16_init(uint32_t offset)
{
	uint32_t data_sector_count, root_directory_sector_count;

	layout.offset = offset;
	int ret = fat16_read_bpb();

	root_directory_sector_count = (bpb.root_entry_count * 32) / bpb.bytes_per_sector;
#ifdef FAT_DEBUG
	printf("FAT16: ROOT DIRECTORY SECTOR COUNT: ");
	print_hex32(root_directory_sector_count);
	printf("\n");
#endif

	data_sector_count = bpb.sector_count - (bpb.reserved_sector_count + (bpb.num_fats * bpb.fat_size) 
			+ root_directory_sector_count);
	layout.data_cluster_count = data_sector_count / bpb.sectors_per_cluster;

	if(layout.data_cluster_count < 4085 || layout.data_cluster_count >= 65525)
		return 100;

	layout.start_fat_region = bpb.reserved_sector_count;
	layout.start_fat_region *= bpb.bytes_per_sector;
	layout.start_root_directory_region = bpb.num_fats;
	layout.start_root_directory_region *= bpb.fat_size;
	layout.start_root_directory_region *= bpb.bytes_per_sector;
	layout.start_root_directory_region += layout.start_fat_region;
	layout.start_data_region = root_directory_sector_count;
	layout.start_data_region *= bpb.bytes_per_sector;
	layout.start_data_region += layout.start_root_directory_region;

	printf("FAT16: FILE SYSTEM LAYOUT:\n");
	printf("   START_FAT_REGION=");
	print_hex32(layout.start_fat_region);
	printf("\n");
	printf("   START_ROOT_DIRECTORY_REGION=");
	print_hex32(layout.start_root_directory_region);
	printf("\n");
	printf("   START_DATA_REGION=");
	print_hex32(layout.start_data_region);
	printf("\n");
	printf("   START_FAT_REGION=");
	print_hex32(layout.start_fat_region);
	printf("\n");
	printf("   DATA_CLUSTER_COUNT=");
	print_hex32(layout.data_cluster_count);
	printf("\n");

	return 0;
}

static uint32_t move_to_root_directory_region(uint16_t entry_index)
{
	uint32_t pos = layout.start_root_directory_region;
	pos += layout.offset;
	pos += entry_index * 32;
	//printf("FAT16: ROOT MOVING TO: ");
	//print_hex32(pos);
	//printf("\n");
	seek(pos);
	return pos;
}

static uint32_t move_to_fat_region(uint16_t cluster)
{
	uint32_t pos = layout.start_fat_region;
	pos += layout.offset;
	pos += cluster * 2;
	//printf("FAT16: FAT MOVING TO: ");
	//print_hex32(pos);
	//printf("\n");
	seek(pos);
	return pos;
}

static uint32_t move_to_data_region(uint16_t cluster, uint16_t offset)
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
	seek(pos);
	return pos;
}

static int read_entry_from_subdir(DirEntry* entry, EntryHandle* handle)
{
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
	read(entry, sizeof(DirEntry));
	handle->offset += sizeof(DirEntry);

	return 0;
}

static int find_entry_in_subdir(DirEntry* entry, uint32_t* entry_pos, EntryHandle* handle, char* name)
{
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

	if(ret == 0 && entry_pos != (void*) 0x0)
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

int get_next_cluster(uint16_t* next_cluster, uint16_t cluster)
{
	move_to_fat_region(cluster);
	read(next_cluster, sizeof(cluster));

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

		read(&bytes[bytes_read_count], chunk_length);

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

bool fat16_read(const char* filepath, char mode, void* buffer, uint32_t count)
{
	char filename[11];

	EntryHandle handle;
	if(navigate_to_subdir(&handle, filename, filepath) < 0)
	{
		return false;
	}
	if(open_file_in_subdir(&handle, filename, mode) < 0)
	{
		return false;
	}

	return read_from_handle(&handle, buffer, count);
}

bool navigate_to_subdir(EntryHandle* handle, char* entry_name, const char* path)
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

int open_file_in_subdir(EntryHandle* handle, char* filename, char mode)
{
	return open_entry_in_subdir(handle, filename, mode, true);
}

int open_directory_in_subdir(EntryHandle* handle, char* dirname)
{
	return open_entry_in_subdir(handle, dirname, 'r', false);
}

int to_short_filename(char* short_filename, const char* long_filename)
{
	uint8_t i = 0;
	uint8_t sep = 0;

	if(long_filename[0] == '/') long_filename++;

	if(long_filename[0] == '\0') return -1;

	for(i = 0; i < 9; i++)
	{
		if(long_filename[i] == '\0')
		{
			memset(&short_filename[i], ' ', 11 - i);
			return 0;
		}

		if(long_filename[i] == '.')
		{
			sep = i;
			break;
		}

		short_filename[i] = long_filename[i];
	}

	if(i == 9) return -1;

	memset(&short_filename[sep], ' ', 8 - sep);

	for(i = 0; i < 3; i++)
	{
		if(long_filename[sep + 1 + i] == '\0')
		{
			break;
		}

		short_filename[8 + i] = long_filename[sep + 1 + i];
	}

	if(i == 3 && long_filename[sep + 4] != '\0') return -1;

	memset(&short_filename[8 + i], ' ', 3 - i);

	return 0;
}

int get_subdir(char* subdir_name, uint16_t* index, const char* path)
{
	const uint16_t beg = *index;
	uint32_t len = 0;

	if(path[beg] != '/')
		return -1;

	len++;
	while(path[beg + len] != '\0')
	{
		if(path[beg + len] == '/') break;

		len++;
	}

	if(path[beg + len] != '/')
		return -2;

	memcpy(subdir_name, &path[beg], len);
	subdir_name[len] = '\0';

	*index = beg + len;
	return 0;
}

int ls_in_subdir(uint32_t* index, char* name, EntryHandle* handle)
{
	DirEntry entry;
	uint32_t entry_index = *index;

	while(entry_index)
	{
		if(read_entry_from_subdir(&entry, handle) < 0) return -1;

		entry_index--;
	}

	if(read_entry_from_subdir(&entry, handle) < 0) return -1;

	if(entry.name[0] == 0) return 0;

	++*index;
	memcpy(name, entry.name, sizeof(entry.name));

	return 1;
}

static int find_root_directory_entry(uint16_t* entry_index, char* name)
{
	uint16_t i = 0;

	move_to_root_directory_region(0);
	for(i = 0; i < bpb.root_entry_count; i++)
	{
		DirEntry e;
		read(&e, sizeof(DirEntry));

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
	read(&entry, sizeof(DirEntry));

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

	read(&entry, sizeof(DirEntry));
	if(entry.name[0] == 0)
		return 0;

	++*index;
	memcpy(filename, entry.name, 11);

	return 1;
}

static bool is_in_root(const char* path)
{
	char subdir_name[12 + 1];
	uint16_t index = 0;

	return get_subdir(subdir_name, &index, path) < 0;
}

int fat16_ls(uint32_t* index, char* filename, const char* dir_path)
{
	int ret;
	char name[11];

	if(dir_path[1] == '\0')
	{
		ret = ls_in_root(index, name);
	}
	else
	{
		EntryHandle handle;
		char dirname[11];

		if(is_in_root(dir_path))
		{
			if(to_short_filename(dirname, dir_path) < 0) return -1;

			if(open_directory_in_root(&handle, dirname) < 0) return -1;
		}
		else
		{
			if(navigate_to_subdir(&handle, dirname, dir_path) < 0 || open_directory_in_subdir(&handle,
						dirname) < 0) return -1;
		}

		ret = ls_in_subdir(index, name, &handle);
	}

	if(ret == 1)
	{
		uint8_t name_length = 0, ext_length = 0;
		if(name[0] == '.' && name[1] == ' ')
		{
			filename[0] = '.';
			filename[1] = '\0';
			return 1;
		}
		else if(name[0] == '.' && name[1] == '.' && name[2] == ' ')
		{
			filename[0] = '.';
			filename[1] = '.';
			filename[2] = '\0';
			return 1;
		}
		
		for(name_length = 0; name_length < 8; name_length++)
		{
			char c = name[name_length];
			if(c == ' ') break;

			filename[name_length] = c;
		}

		filename[name_length] = '.';

		for(ext_length = 0; ext_length < 3; ext_length++)
		{
			char c = name[8 + ext_length];
			if(c == ' ') break;

			filename[name_length + 1 + ext_length] = c;
		}

		filename[name_length + 1 + ext_length] = '\0';
	}

	return ret;
}
