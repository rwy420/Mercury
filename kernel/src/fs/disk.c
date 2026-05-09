#include <fs/disk.h>
#include <common/screen.h>
#include <driver/ata/ata.h>
#include <memory/heap.h>

int read_mbr()
{
	uint8_t* mbr_buffer = kmalloc(512);
	read28(0, mbr_buffer, 512);

	MBR* mbr = (MBR*) mbr_buffer;

	if(mbr->signature != 0xAA55) return false;

	return true;
}
