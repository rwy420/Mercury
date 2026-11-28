#ifndef __MERCURY__FS__DEVFS__DEVFS_H
#define __MERCURY__FS__DEVFS__DEVFS_H

#include <fs/vfs/vfs.h>

VFSNode** devfs_read_dir(VFSNode* node);

#endif
