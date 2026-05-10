#include <fs/disk.h>
#include <fs/fat/bpb.h>
#include <common/screen.h>
#include <driver/ata/ata.h>
#include <memory/heap.h>
#include <vesa.h>

int read_mbr()
{
	uint8_t* mbr_buffer = kmalloc(512);
	read28(0, 0, mbr_buffer, 512);

	MBR* mbr = (MBR*) mbr_buffer;

	if(mbr->signature != 0xAA55) return false;

	for(uint8_t i = 0; i < 3; i++) 
	{
		PartitionTableEntry* entry = &mbr->partition_table_entries[i];
		if(entry->type == 0) break;

		printf("<MBR> Disk partition ");
		print_uint8_t(i);
		printf(":\n");
		printf("   Type: 0x");
		print_hex(entry->type);
		printf("\n");
		printf("   Size: ");
		print_uint32_t((entry->num_sectors * 512) / 1024 / 1024);
		printf("MB\n");

		if(entry->type == 0x0C)
		{
			if(!fat_read_bpb(entry)) printf_color("<FAT> Could not read the FAT BPB\n", COLOR_RED, COLOR_BLACK);
		}
	}

	return true;
}
