#include <fd.h>
#include <memory/common.h>
#include <common/screen.h>
#include <fs/fat16/fat16.h>

FileDescriptor g_file_descriptors[0xFF];
int fd_count;

FileDescriptor* stdin;
FileDescriptor* stdout;

void fd_init()
{
	stdin = &g_file_descriptors[0];
	stdout = &g_file_descriptors[1];

	stdin->index = 0;
	stdout->index = 1;

	stdout->write = syscall_printf;

	fd_count = 2;
}

FileDescriptor* create_fd()
{
	for(int i = 0; i < 0xFF; i++)
	{
		if(g_file_descriptors[i].index == 0x100) 
		{
			g_file_descriptors[i].index = i;
			return &g_file_descriptors[i];
		}
	}

	return NULL_PTR;
}

void close_fd(int fd)
{
	memset(&g_file_descriptors[fd], 0, sizeof(FileDescriptor));
	g_file_descriptors[fd].index = 0x100;
}

int syscall_read(CPUState* cpu)
{
	uint32_t fd_idx = cpu->ebx;
	char* buffer = (char*) cpu->ecx;
	size_t length = cpu->edx;

	FileDescriptor* fd = &g_file_descriptors[fd_idx];
	if(fd->read != NULL_PTR) 
	{
		fd->read(buffer, length);
	}

	return cpu->eax;
}

int syscall_write(CPUState* cpu)
{
	uint32_t fd_idx = cpu->ebx;
	char* buffer = (char*) cpu->ecx;
	size_t length = cpu->edx;

	FileDescriptor* fd = &g_file_descriptors[fd_idx];
	if(fd->write != NULL_PTR) 
	{
		fd->write(buffer, length);
	}

	return cpu->eax;
}

int syscall_open(CPUState* cpu)
{
	char* path = (char*) cpu->ebx;
	int flags = cpu->ecx;
	char mode = cpu->edx;

	return fat16_open(path, mode);
}

int syscall_close(CPUState* cpu)
{
	uint32_t fd = cpu->ebx;
	fat16_close(fd);

	return cpu->eax;
}

int syscall_lseek(CPUState* cpu)
{
	uint32_t fd_idx = cpu->ebx;
	FileDescriptor* fd = &g_file_descriptors[fd_idx];
	uint32_t offset = cpu->ecx;
	uint32_t whence = cpu->edx;
	uint32_t seek = 0;

	switch(whence)
	{
		case SEEK_SET:
			seek = offset;
			break;
		case SEEK_CUR:
			seek = fd->seek + offset;
			break;
		case SEEK_END: //TODO
			break;
		default:
			break;
	}

	fd->seek = seek;
	return cpu->eax;
}
