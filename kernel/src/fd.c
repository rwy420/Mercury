#include <fd.h>

FileDescriptor g_file_descriptors[0xFF];
int fd_count;

void fd_init()
{
	fd_count = 0;
}

FileDescriptor* create_fd()
{
	for(int i = 0; i < 0xFF; i++)
	{

	}
}

FileDescriptor* get_fd(int fd)
{

}

void close_fd(int fd)
{

}
