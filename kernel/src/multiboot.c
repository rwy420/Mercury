#include <multiboot.h>
#include <printf.h>
#include <panic.h>

multiboot_info_t* mb;

void mb_confirm(uint32_t multiboot, uint32_t magic)
{
	mb = (multiboot_info_t*) multiboot;
	if(magic != MULTIBOOT_BOOTLOADER_MAGIC) panic("Multiboot header is invalid", 0);
}

void mb_get_vesa(VesaInfo* vesa_info)
{
	vesa_info->fb = mb->framebuffer_addr;
	vesa_info->fb_height = mb->framebuffer_height;
	vesa_info->fb_width = mb->framebuffer_width;
}

multiboot_module_t* mb_init_modules()
{
	kprintf("<GRUB> Loaded %u module(s)\n", mb->mods_count);

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
