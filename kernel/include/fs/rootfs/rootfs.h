#ifndef __MERCURY__FS__ROOTFS__ROOTFS_H
#define __MERCURY__FS__ROOTFS__ROOTFS_H

#include <fs/vfs/vfs.h>

#define ROOTFS_MAX_CHILDREN 32

typedef struct RootFSNode
{
	char name[32];
	uint8_t attributes;
	struct RootFSNode* children[ROOTFS_MAX_CHILDREN];
} RootFSNode;

VFSMount* rootfs_mount();
void* rootfs_lookup(VFSNode* dir, char* name);
void* rootfs_create_dir(VFSNode* parent, char* name);

#endif
