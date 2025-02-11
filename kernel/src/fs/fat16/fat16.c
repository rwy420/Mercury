#include <fs/fat16/fat16.h>
#include <fs/fat16/fat16_priv.h>
#include <memory/common.h>
#include <core/screen.h>

#include <driver/ata/ata.h>

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

	printf("FAT16: #####   BPB   #####\n");
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
	printf("FAT16: OEM NAME: ");
	printf(bpb.oem_name);
	printf("\n");

	dev->read(&bpb.bytes_per_sector, 2);
	printf("FAT16: BYTES PER SECTOR: ");
	print_hex((bpb.bytes_per_sector >> 8) & 0xFF);
	print_hex(bpb.bytes_per_sector & 0xFF);
	printf("\n");

	if(bpb.bytes_per_sector != 512) return INVALID_BYTES_PER_SECTOR;

	dev->read(&bpb.sectors_per_cluster, 1);
	printf("FAT16: SECTORS PER CLUSTER ");
	print_hex(bpb.sectors_per_cluster);
	printf("\n");
	if(bpb.sectors_per_cluster != 1 && bpb.sectors_per_cluster != 2 && bpb.sectors_per_cluster != 4
			&& bpb.sectors_per_cluster != 8 && bpb.sectors_per_cluster != 16 && bpb.sectors_per_cluster != 32
			&& bpb.sectors_per_cluster != 64 && bpb.sectors_per_cluster != 128)
		return INVALID_SECTORS_PER_CLUSTER;

	dev->read(&bpb.reserved_sector_count, 2);
	printf("FAT16: RESERVED SECTOR COUNT: ");
	print_hex((bpb.reserved_sector_count >> 8) & 0xFF);
	print_hex(bpb.reserved_sector_count & 0xFF);
	printf("\n");

	dev->read(&bpb.num_fats, 1);
	printf("FAT16: NUM FATS: ");
	print_hex(bpb.num_fats);
	printf("\n");

	dev->read(&bpb.root_entry_count, 2);
	printf("FAT16: ROOT ENTRY COUNT: ");
	print_hex((bpb.root_entry_count >> 8) & 0xFF);
	print_hex(bpb.root_entry_count & 0xFF);
	printf("\n");

	dev->read(&bpb.sector_count, 2);
	
	dev->read_byte(&data);

	dev->read(&bpb.fat_size, 2);
	printf("FAT16: FAT SIZE: ");
	print_hex((bpb.fat_size >> 8) & 0xFF);
	print_hex(bpb.fat_size & 0xFF);
	printf("\n");

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
		printf("FAT16: VOLUME ID: ");
		print_hex32(bpb.volume_id);
		printf("\n");

		dev->read(&bpb.label, 11);
		printf("FAT16: LABEL: ");
		printf(bpb.label);
		printf("\n");

		dev->read(&bpb.fs_type, 8);
		printf("FAT16: FS TYPE: ");
		printf(bpb.fs_type);
		printf("\n");
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
	printf("FAT16: ROOT DIRECTORY SECTOR COUNT: ");
	print_hex32(root_directory_sector_count);
	printf("\n");

	return 0;
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
