#include <fs/fat32/fat32.h>
#include <memory/heap.h>
#include <common/screen.h>
#include <fs/fat/fat.h>
#include <driver/ata/ata.h>
#include <fs/fat32/fat32_dir.h>

FAT32Volume g_volume;

int fat32_init(BPB* bpb, EBPB_FAT32* ebpb, PartitionTableEntry* partition)
{
	g_volume.bpb = *bpb;
	g_volume.ebpb = *ebpb;
	g_volume.partition = *partition;

	g_volume.fat_start = partition->lba_start + bpb->num_reserved_sectors;
	g_volume.data_start = g_volume.fat_start + (bpb->num_fats * ebpb->num_sectors_per_fat);
	g_volume.root_cluster = ebpb->root_dir_cluster;
	g_volume.sectors_per_cluster = bpb->num_sectors_per_cluster;

	return true;
}

int fat32_open(FileDescriptor* fd, char* path)
{
	FATDirectoryEntry* entry = fat_path_to_dir_entry(g_volume.root_cluster, path);
	if(entry == 0) return 1;

	FAT32File* file = kmalloc(sizeof(FAT32File));
	file->entry = entry;

	fd->type = FD_FAT32_FILE;
	fd->object = file;

	fd->read = fat32_read;
	fd->write = fat32_write;
	fd->close = fat32_close;	

	return 0;
}

int fat32_read(void* file_object, void* buffer, size_t length)
{

}

int fat32_write(void* file_object, void* buffer, size_t length)
{

}

int fat32_close(void* file_object)
{
	kfree(file_object);
	return 0;
}
