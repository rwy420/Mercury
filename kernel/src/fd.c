#include <fd.h>
#include <memory/common.h>
#include <common/screen.h>

FileDescriptor g_file_descriptors[0xFF];

FileDescriptor* stdin;
FileDescriptor* stdout;

void fd_init()
{
	stdin = &g_file_descriptors[0];
	stdout = &g_file_descriptors[1];

	stdin->id = 0;
	stdout->id = 1;

	memset(g_file_descriptors, 0, sizeof(g_file_descriptors));

	stdout->write = syscall_printf;
}

FileDescriptor* create_fd()
{
	for(int i = 0; i < 0xFF; i++)
	{
		if(g_file_descriptors[i].attributes == 0x00)
		{
			g_file_descriptors[i].attributes = 0x01;
			g_file_descriptors[i].id = i;
			return &g_file_descriptors[i];
		}
	}

	return 0;
}

void close_fd(int fd)
{
	memset(&g_file_descriptors[fd], 0, sizeof(FileDescriptor));
	g_file_descriptors[fd].attributes = 0x01;
}

int syscall_read(CPUState* cpu)
{
	uint32_t fd_idx = cpu->ebx;
	char* buffer = (char*) cpu->ecx;
	size_t length = cpu->edx;

	FileDescriptor* fd = &g_file_descriptors[fd_idx];
	if(fd->read != NULL_PTR) 
	{
		fd->read(fd->object, buffer, fd->offset, length);
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
		fd->write(fd->object, buffer, fd->offset, length);
	}

	return cpu->eax;
}

int syscall_open(CPUState* cpu)
{
	char* path = (char*) cpu->ebx;
	int flags = cpu->ecx;
	char mode = cpu->edx;

	return 0; //fat16_open(path, mode);
}

int syscall_close(CPUState* cpu)
{
	uint32_t fd = cpu->ebx;
	//fat16_close(fd);

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
			seek = fd->offset + offset;
			break;
		case SEEK_END: //TODO
			break;
		default:
			break;
	}

	fd->offset = seek;
	return cpu->eax;
}
