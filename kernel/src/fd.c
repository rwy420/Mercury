#include "vm.h"
#include <fd.h>
#include <memory/common.h>
#include <memory/heap.h>
#include <vesa.h>

FileDescriptor g_file_descriptors[MAX_FD_COUNT];

FileDescriptor* stdin;
FileDescriptor* stdout;

VFSNodeOps stdout_ops = {0, 0, 0,  syscall_printf, 0, 0};

void fd_init()
{
	memset(g_file_descriptors, 0, sizeof(g_file_descriptors));
	
	stdin = create_fd();
	stdout = create_fd();

	stdout->node->ops = &stdout_ops;
}

FileDescriptor* create_fd()
{
	for(int i = 0; i < MAX_FD_COUNT; i++)
	{
		if(g_file_descriptors[i].attributes == 0x00)
		{
			g_file_descriptors[i].attributes = 0x01;
			g_file_descriptors[i].id = i;
			VFSNode* node = (VFSNode*) kmalloc(sizeof(VFSNode));
			g_file_descriptors[i].node = node;
			memset(node, 0, sizeof(VFSNode));
			return &g_file_descriptors[i];
		}
	}

	return 0;
}

void close_fd(int fd)
{
	memset(&g_file_descriptors[fd], 0, sizeof(FileDescriptor));
	g_file_descriptors[fd].attributes = 0x00;
}

int syscall_read(CPUState* cpu)
{
	uint32_t fd_idx = cpu->ebx;
	if(fd_idx > MAX_FD_COUNT) return 0;

	char* buffer = (char*) cpu->ecx;
	size_t length = cpu->edx;

	FileDescriptor* fd = &g_file_descriptors[fd_idx];

	if(!fd) return 0; //TODO: ERRORS
	if(!fd->node->ops || !fd->node->ops->read) return 0;

	fd->node->ops->read(fd->node->fs_object, buffer, fd->offset, length);

	return length; //TODO
}

int syscall_write(CPUState* cpu)
{
	uint32_t fd_idx = cpu->ebx;
	if(fd_idx > MAX_FD_COUNT) return 0;

	char* buffer = (char*) cpu->ecx;
	size_t length = cpu->edx;

	FileDescriptor* fd = &g_file_descriptors[fd_idx];
	
	if(!fd) return 0; //TODO: ERRORS
	if(!fd->node->ops || !fd->node->ops->write) return 0;

	fd->node->ops->write(fd->node->fs_object, buffer, fd->offset, length);

	return length; //TODO
}

int syscall_open(CPUState* cpu)
{
	//TODO
	return 0;
}

int syscall_close(CPUState* cpu)
{
	//TODO
	uint32_t fd = cpu->ebx;

	return 0;
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
	return 0; //TODO
}
