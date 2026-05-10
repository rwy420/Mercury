#include <common/screen.h>
#include <fs/fat/bpb.h>
#include <memory/heap.h>
#include <driver/ata/ata.h>

int fat_read_bpb(PartitionTableEntry* partition)
{
	uint8_t* bpb_buffer = kmalloc(sizeof(BPB));
	read28(partition->lba_start, 0, bpb_buffer, sizeof(BPB));

	BPB* bpb = (BPB*) bpb_buffer;

	if(bpb->jmp_short[0] != 0xEB) return false;

	printf("   <FAT> Has valid BPB\n");

	return true;
}
