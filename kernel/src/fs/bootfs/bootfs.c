#include <core/screen.h>
#include <fs/bootfs/bootfs.h>
#include <driver/disk/ata.h>
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
<<<<<<< HEAD
	printf(entries[0].name);
=======
>>>>>>> d183a15 (ELF32 Loader working)
}
