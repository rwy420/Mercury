#ifndef __MERCURY__MEMORY__FRAMES_H
#define __MERCURY__MEMORY__FRAMES_H

void frame_allocator_init();
void* alloc_frame();
void free_frame(void* frame);

#endif
