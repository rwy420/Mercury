#include <fs/fs.h>
#include <fd.h>
#include <fs/fat32/fat32.h>
#include <common/screen.h>

extern FileDescriptor g_file_descriptors[];

uint32_t fs_open(char* path)
{
	FileDescriptor* fd = create_fd();
	if(fat32_open(fd, path) != 0) return 0;

	return fd->id;
}

int fs_read(uint32_t fd_id, void* buffer, size_t length)
{
	FileDescriptor* fd = &g_file_descriptors[fd_id];
	int code = fat32_read(fd->object, buffer, fd->offset, length);
	fd->offset += length;

	return code;
}

int fs_write(uint32_t fd_id, void* buffer, size_t length)
{

}

int fs_close(uint32_t fd_id)
{

}

int fs_seek(uint32_t fd_id, uint32_t offset)
{

}
