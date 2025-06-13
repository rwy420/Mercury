#include <memory/heap.h>
#include <memory/frames.h>
#include <memory/common.h>

static uint8_t* frame_bitmap;
uint32_t total_frames;
uint32_t frames_start;

static inline void set_frame(uint32_t address)
{
	uint32_t frame = address / FRAME_SIZE;
	frame_bitmap[FRAME_INDEX(frame)] |= (1 << FRAME_OFFSET(frame));
}

static inline void clear_frame(uint32_t address)
{
	uint32_t frame = address / FRAME_SIZE;
	frame_bitmap[FRAME_INDEX(frame)] &= ~(1 << FRAME_OFFSET(frame));
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
		uint32_t byte = frame_bitmap[i / 8];
		
		if(!(byte & (1 << (i % 8))))
		{
			frame_bitmap[i / 8] |= (1 << (i % 8));

			return (void*) (frames_start + i * FRAME_SIZE); 
		}
	}

	return NULL_PTR;
}

void free_frame(void* frame)
{
	clear_frame((uint32_t) frame);
}
