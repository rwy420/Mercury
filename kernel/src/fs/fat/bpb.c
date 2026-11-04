#include <common/screen.h>
#include <fs/fat/bpb.h>
#include <fs/fat32/fat32.h>
#include <memory/heap.h>
#include <driver/ata/ata.h>
#include <memory/common.h>

int fat_read_bpb(BPB* bpb_buffer, PartitionTableEntry* partition)
{
	uint8_t* buffer = kmalloc(512);
	read28(partition->lba_start, 0, (uint8_t*) buffer, 512);
	memcpy(bpb_buffer, buffer, 512);
	kfree(buffer);

	BPB* bpb = (BPB*) bpb_buffer;
	uint8_t* ebpb_buffer = (uint8_t*) bpb_buffer + sizeof(BPB);

	if(bpb->jmp_short[0] != 0xEB) return -1;

	printf("   <FAT> Has valid BPB\n");

	if(bpb->large_num_sectors != 0) return 1;
	
	return 0;
}
