#ifndef __MERCURY__MEMORY__FRAMES_H
#define __MERCURY__MEMORY__FRAMES_H

#define FRAME_SIZE 0x1000

#include <common/types.h>

int frame_allocator_init(uint32_t start, size_t size);
void* alloc_frame();
void free_frame(void* frame);
void* alloc_frames(size_t count);
void free_frames(void* frame, size_t count);

#endif
