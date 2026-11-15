#include <fs/disk.h>
#include <fs/fat/bpb.h>
#include <fs/fat/fat.h>
#include <fs/fat32/fat32.h>
#include <driver/ata/ata.h>
#include <memory/heap.h>
#include <printf.h>

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

		kprintf("<MBR> Disk partition %u:\n   Type: 0x%x\n   Size: %u MB\n", (uint32_t) i, (uint32_t) entry->type, (uint32_t) (entry->num_sectors
					* 512) / 1024 / 1024);

		if(entry->type == 0x0C)
		{
			BPB* bpb_buffer = kmalloc(512);
			int fat_type = fat_read_bpb(bpb_buffer, entry);

			switch(fat_type)
			{
				case -1:
					kprintf("<FAT> Could not read the FAT BPB\n");
					break;
				case 0:
					break;
				case 1:
					uint8_t* ebpb_buffer = (uint8_t*) bpb_buffer + sizeof(BPB);
					EBPB_FAT32* ebpb = (EBPB_FAT32*) ebpb_buffer;
					VFSNode* fat32_root = fat32_mount(bpb_buffer, ebpb, entry);
					vfs_mount_root(fat32_root);
					break;
			}
		}
	}

	return true;
}
