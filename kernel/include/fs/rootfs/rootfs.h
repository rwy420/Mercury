#ifndef __MERCURY__FS__ROOTFS__ROOTFS_H
#define __MERCURY__FS__ROOTFS__ROOTFS_H

#include <fs/vfs/vfs.h>

typedef struct RootFSNode
{
	char name[32];
	struct RootFSNode* children[32];
} RootFSNode;

VFSMount* rootfs_mount();
void* rootfs_lookup(VFSNode* dir, char* name);
void* rootfs_create_dir(VFSNode* parent, char* name);

#endif
