#ifndef __MERCURY__COMMON__SCREEN_H
#define __MERCURY__COMMON__SCREEN_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <fs/vfs/vfs.h>

int syscall_printf(VFSNode* node, void* buffer, uint32_t offset, size_t length);

void uint8_t_to_string(uint8_t value, char* buffer);
void uint32_t_to_string(uint32_t value, char* buffer);

#endif
