#include <memory/heap.h>
#include <memory/frames.h>
#include <memory/common.h>

static uint8_t* frame_bitmap;
uint32_t total_frames;
uint32_t frames_start;

static inline void set_frame(uint32_t i)
{
	frame_bitmap[i / 8] |= (1 << (i % 8));
}

static inline void clear_frame(uint32_t i)
{
	frame_bitmap[i / 8] &= ~(1 << (i % 8));
}

static inline int frame_used(uint32_t i)
{
	return (frame_bitmap[i / 8] >> (i % 8)) & 1;
}

int frame_allocator_init(uint32_t start, size_t size)
{
	total_frames = size / FRAME_SIZE;
	frames_start = start;

	frame_bitmap = kmalloc(total_frames / 8);
	if(!frame_bitmap) return false;

	memset(frame_bitmap, 0, size / FRAME_SIZE / 8);

	return true;
}

void* alloc_frame()
{
	for(uint32_t i = 0; i < total_frames; i++)
	{
		if(!frame_used(i))
		{
			set_frame(i);
			return (void*) (frames_start + i * FRAME_SIZE); 
		}
	}

	return NULL_PTR;
}

void free_frame(void* frame)
{
	uint32_t index = ((uint32_t) frame - frames_start) / FRAME_SIZE;
	clear_frame(index);
}

void* alloc_frames(size_t count)
{
	if (count == 0 || count > total_frames) return NULL_PTR;

	for (uint32_t i = 0; i <= total_frames - count; i++) 
	{
		int found = 1;

		for (uint32_t j = 0; j < count; j++)
		{
			if (frame_used(i + j))
			{
				found = 0;
				i += j;
				break;
			}
		}

		if (found) 
		{
			for (uint32_t j = 0; j < count; j++) set_frame(i + j);

			return (void*)(frames_start + i * FRAME_SIZE);
		}
	}

	return NULL_PTR;
}

void free_frames(void* frame, size_t count)
{
	uint32_t index = ((uint32_t) frame - frames_start) / FRAME_SIZE;

	for(uint32_t i = 0; i  < count; i++) clear_frame(index + i);
}
