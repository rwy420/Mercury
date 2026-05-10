#include <common/screen.h>
#include <fs/fat/bpb.h>
#include <fs/fat32/fat32.h>
#include <memory/heap.h>
#include <driver/ata/ata.h>

int fat_read_bpb(PartitionTableEntry* partition)
{
	uint8_t* bpb_buffer = kmalloc(512);
	read28(partition->lba_start, 0, bpb_buffer, 512);

	BPB* bpb = (BPB*) bpb_buffer;
	uint8_t* ebpb_buffer = bpb_buffer + sizeof(BPB);

	if(bpb->jmp_short[0] != 0xEB) return false;

	printf("   <FAT> Has valid BPB\n");

	if(bpb->large_num_sectors != 0)
	{
		EBPB_FAT32* ebpb = (EBPB_FAT32*) ebpb_buffer;
		fat32_init(bpb, ebpb, partition);
	}
	
	return true;
}
