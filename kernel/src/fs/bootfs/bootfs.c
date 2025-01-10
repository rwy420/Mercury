#include <core/screen.h>
#include <fs/bootfs/bootfs.h>
#include <driver/ata/ata.h>
#include <memory/mem_manager.h>

BootFS_Entry* entries;

void read_files()
{
	uint8_t* buffer = (uint8_t*) alloc_blocks(1);

	for(int i = 0; i < 8; i++)
	{
		uint8_t* current_buffer = buffer + (i * 512);
		read28(i + 1, current_buffer, 512);
	}

	entries = (BootFS_Entry*) buffer;
	
	for(int i = 0; i < 2; i++)
	{
		BootFS_Entry* entry = &entries[i];

		/*
		printf("<Mercury> Found BootFS entry: ");
		printf(entry->name);
		printf(" ranging from disk sector 0x");
		print_hex32(entry->start);
		printf(" to 0x");
		print_hex32(entry->end);
		printf("\n");
		printf(entry->type);
		*/
	}
}
