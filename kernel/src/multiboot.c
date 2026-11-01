#include <multiboot.h>
#include <common/screen.h>

multiboot_info_t* mb;

int mb_confirm(uint32_t multiboot, uint32_t magic)
{
	mb = (multiboot_info_t*) multiboot;

	return 0;
}

void mb_get_vesa(VesaInfo* vesa_info)
{
	vesa_info->fb = mb->framebuffer_addr;
	vesa_info->fb_height = mb->framebuffer_height;
	vesa_info->fb_width = mb->framebuffer_width;
}

multiboot_module_t* mb_init_modules()
{
	printf("<GRUB> Loaded ");
	print_uint32_t(mb->mods_count);
	printf(" module(s)\n");

	return (multiboot_module_t*) mb->mods_addr;
}

uint8_t mb_module_count()
{
	return mb->mods_count;
}

multiboot_module_t* mb_get_module(uint8_t idx)
{
	if(idx >= mb->mods_count) return 0;

	multiboot_module_t* mods = (multiboot_module_t*) mb->mods_addr;
	return &mods[idx];
}
