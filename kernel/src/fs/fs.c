#include <fs/fs.h>
#include <fd.h>
#include <fs/fat32/fat32.h>

uint32_t fs_open(char* path)
{
	FileDescriptor* fd = create_fd();
	if(fat32_open(fd, path) != 0) return 0;

	return fd->id;
}

int fs_read(uint32_t fd, void* buffer, size_t length)
{

}

int fs_write(uint32_t fd, void* buffer, size_t length)
{

}

int fs_close(uint32_t fd)
{

}

int fs_seek(uint32_t fd, uint32_t offset)
{

}
