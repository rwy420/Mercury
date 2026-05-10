#include <common/screen.h>
#include <fs/fat/bpb.h>
#include <memory/heap.h>
#include <driver/ata/ata.h>

int fat_read_bpb(uint32_t sector)
{
	uint8_t* bpb_buffer = kmalloc(sizeof(BPB));
	read28(sector, 0, bpb_buffer, sizeof(BPB));

	BPB* bpb = (BPB*) bpb_buffer;

	if(bpb->jmp_short[0] != 0xEB) return false;

	return true;
}
