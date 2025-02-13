#include "fs/fat16/fat16_subdir.h"
#include <fs/fat16/fat16.h>
#include <fs/fat16/fat16_priv.h>
#include <fs/fat16/fat16_rootdir.h>
#include <fs/fat16/fat16_path.h>
#include <memory/common.h>
#include <core/screen.h>
#include <driver/ata/ata.h>

//#define FAT_DEBUG

#define INVALID_HANDLE 255
#define HANDLE_COUNT 16
#define SECTOR_SIZE 512
uint32_t disk_pointer = 0;

FAT16BPB bpb;
static EntryHandle handles[HANDLE_COUNT];
FAT16Layout layout;
storage_dev_t* dev;

int fat16_read_bpb()
{
	uint8_t data;
	memset(&bpb, 0, sizeof(FAT16BPB));
	dev->seek(layout.offset);

#ifdef FAT_DEBUG 
	printf("FAT16: #####   BPB   #####\n");
#endif
	dev->read_byte(&data);
	if(data == 0xEB)
	{
		dev->read_byte(&data);
		dev->read_byte(&data);
		if(data != 0x90)
		{
			return INAVLID_JUMP_INSTRUCTION;
		}
	}
	else if(data == 0xE9)
	{
		dev->read_byte(&data);
		dev->read_byte(&data);
	}
	else
	{
		return INAVLID_JUMP_INSTRUCTION;
	}

	dev->read(&bpb.oem_name, 8);
#ifdef FAT_DEBUG
	printf("FAT16: OEM NAME: ");
	printf(bpb.oem_name);
	printf("\n");
#endif

	dev->read(&bpb.bytes_per_sector, 2);
#ifdef FAT_DEBUG
	printf("FAT16: BYTES PER SECTOR: ");
	print_hex((bpb.bytes_per_sector >> 8) & 0xFF);
	print_hex(bpb.bytes_per_sector & 0xFF);
	printf("\n");
#endif

	if(bpb.bytes_per_sector != 512) return INVALID_BYTES_PER_SECTOR;

	dev->read(&bpb.sectors_per_cluster, 1);
#ifdef FAT_DEBUG
	printf("FAT16: SECTORS PER CLUSTER ");
	print_hex(bpb.sectors_per_cluster);
	printf("\n");
#endif

	if(bpb.sectors_per_cluster != 1 && bpb.sectors_per_cluster != 2 && bpb.sectors_per_cluster != 4
			&& bpb.sectors_per_cluster != 8 && bpb.sectors_per_cluster != 16 && bpb.sectors_per_cluster != 32
			&& bpb.sectors_per_cluster != 64 && bpb.sectors_per_cluster != 128)
		return INVALID_SECTORS_PER_CLUSTER;

	dev->read(&bpb.reserved_sector_count, 2);
#ifdef FAT_DEBUG
	printf("FAT16: RESERVED SECTOR COUNT: ");
	print_hex((bpb.reserved_sector_count >> 8) & 0xFF);
	print_hex(bpb.reserved_sector_count & 0xFF);
	printf("\n");
#endif

	dev->read(&bpb.num_fats, 1);
#ifdef FAT_DEBUG
	printf("FAT16: NUM FATS: ");
	print_hex(bpb.num_fats);
	printf("\n");
#endif

	dev->read(&bpb.root_entry_count, 2);
#ifdef FAT_DEBUG
	printf("FAT16: ROOT ENTRY COUNT: ");
	print_hex((bpb.root_entry_count >> 8) & 0xFF);
	print_hex(bpb.root_entry_count & 0xFF);
	printf("\n");
#endif

	dev->read(&bpb.sector_count, 2);
	
	dev->read_byte(&data);

	dev->read(&bpb.fat_size, 2);
#ifdef FAT_DEBUG
	printf("FAT16: FAT SIZE: ");
	print_hex((bpb.fat_size >> 8) & 0xFF);
	print_hex(bpb.fat_size & 0xFF);
	printf("\n");
#endif

	dev->read_byte(&data);
	dev->read_byte(&data);

	dev->read_byte(&data);
	dev->read_byte(&data);


	dev->read_byte(&data);
	dev->read_byte(&data);
	dev->read_byte(&data);
	dev->read_byte(&data);

	uint32_t sector_count_32;
	dev->read(&sector_count_32, 4);
	if((bpb.sector_count != 0 && sector_count_32 != 0) || (bpb.sector_count == 0 && sector_count_32 == 0))
		return INVALID_SECTOR_COUNT;

	dev->read_byte(&data);
	dev->read_byte(&data);

	dev->read_byte(&data);
	if(data == 0x29)
	{
		dev->read(&bpb.volume_id, 4);
#ifdef FAT_DEBUG
		printf("FAT16: VOLUME ID: ");
		print_hex32(bpb.volume_id);
		printf("\n");
#endif

		dev->read(&bpb.label, 11);
#ifdef FAT_DEBUG
		printf("FAT16: LABEL: ");
		printf(bpb.label);
		printf("\n");
#endif

		dev->read(&bpb.fs_type, 8);
#ifdef FAT_DEBUG
		printf("FAT16: FS TYPE: ");
		printf(bpb.fs_type);
		printf("\n");
#endif
	}

	return 0;
}

int fat16_init(storage_dev_t* _dev, uint32_t offset)
{
	disk_pointer = 0;
	uint32_t data_sector_count, root_directory_sector_count;

	dev = _dev;
	layout.offset = offset;
	int ret = fat16_read_bpb();

	if(ret > 0)
		return ret;

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
#ifdef FAT_DEBUG
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
#endif

	memset(handles, 0, sizeof(handles));

	return 0;
}

static uint8_t find_available_handle()
{
	uint8_t i = 0;

	for(; i < HANDLE_COUNT; i++)
	{
		if(handles[i].mode == 0)
		{
			return i;
		}
	}	

	return INVALID_HANDLE;
}

static bool check_handle(uint8_t handle)
{
	if(handle >= HANDLE_COUNT) return false;
	if(handles[handle].mode == 0) return false;

	return true;
}

int fat16_open(const char* filepath, char mode)
{
	int i;
	char filename[11];
	uint8_t handle = INVALID_HANDLE;

	if(mode != 'r' && mode != 'w' && mode != 'a')
	{
		return -1;
	}

	handle = find_available_handle();

	if(is_in_root(filepath))
	{
		if(to_short_filename(filename, filepath) < 0) return -1;

		//TODO HANDLE MODES
		if(mode == 'w')
		{

		}
		else if(mode == 'a')
		{

		}

		if(open_file_in_root(&handles[handle], filename, mode) < 0)
		{
			handles[handle].mode = 0;
			return -1;
		}
		
		return 0;
	}
	else
	{
		EntryHandle dir_handle;
		if(navigate_to_subdir(&dir_handle, filename, filepath) < 0) return -1;

		//HANDLE MODES
		if(mode == 'w')
		{

		}
		else if(mode == 'a')
		{

		}

		if(open_file_in_subdir(&dir_handle, filename, mode) < 0) return -1;

		handles[handle] = dir_handle;
	}

	for(i = 0; i < HANDLE_COUNT; i++)
	{
		if(handles[i].mode == 0) continue;
		if(i == handle) continue;

		if(handles[handle].pos_entry == handles[i].pos_entry)
		{
			if((mode == 'r' && handles[i].mode != 'r') || mode != 'r')
			{
				handles[handle].mode = 0;
				return -1;
			}
		}
	}

	return handle;
}

int fat16_close(uint8_t handle)
{
	if(check_handle(handle) == false)
	{
		return -1;
	}

	handles[handle].mode = 0;
	return 0;
}

int fat16_read(uint8_t handle, void* buffer, uint32_t count)
{
	if(check_handle(handle) == false)
	{
		return -1;
	}

	if(buffer == NULL_PTR)
	{
		return -1;
	}

	return read_from_handle(&handles[handle], buffer, count);
}

int fat16_ls(uint32_t* index, char* filename, const char* dir_path)
{
	int ret;
	char name[11];

	if(index == NULL_PTR || filename == NULL_PTR)
		return -1;

	if(dir_path == NULL_PTR || dir_path[0] != '/')
		return -1;

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































int _read(void* buffer, uint32_t length)
{
    uint8_t sector_buffer[SECTOR_SIZE];
    uint8_t* buf = (uint8_t*) buffer;

    uint32_t sector = disk_pointer / SECTOR_SIZE;     // Starting sector
	sector += 100;
    uint32_t offset = disk_pointer % SECTOR_SIZE;     // Byte offset in that sector

    uint32_t remaining = length;

    // Handle first partial sector
    if (offset != 0) {
        read28(sector, sector_buffer, 512);
        uint32_t to_copy = (remaining < (SECTOR_SIZE - offset)) ? remaining : (SECTOR_SIZE - offset);
        memcpy(buf, sector_buffer + offset, to_copy);
        buf += to_copy;
        remaining -= to_copy;
        sector++;
        disk_pointer += to_copy;
    }

    // Read full sectors
    while (remaining >= SECTOR_SIZE) {
        read28(sector, buf, 512);
        buf += SECTOR_SIZE;
        remaining -= SECTOR_SIZE;
        sector++;
        disk_pointer += SECTOR_SIZE;
    }

    // Handle last partial sector
    if (remaining > 0) {
        read28(sector, sector_buffer, 512);
        memcpy(buf, sector_buffer, remaining);
        disk_pointer += remaining;
    }
	
	return 0;
}

int _read_byte(void* buffer)
{
	return _read(buffer, 1);
}

int _write(const void* buffer, uint32_t length)
{
	return 0;
}

int _seek(uint32_t offset)
{
	disk_pointer = offset;
	return 0;
}
