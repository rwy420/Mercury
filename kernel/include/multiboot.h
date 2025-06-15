#ifndef __MERCURY__MULTIBOOT_H
#define __MERCURY__MULTIBOOT_H

#include <common/types.h>

#define MULTIBOOT_MEMORY_AVAILABLE        1
#define MULTIBOOT_MEMORY_RESERVED         2

#define MULTIBOOT_BOOTLOADER_MAGIC        0x2BADB002
#define MULTIBOOT_HEADER_MAGIC            0x1BADB002

#define MULTIBOOT_FLAG_MEM_INFO           (1 << 1)
#define MULTIBOOT_FLAG_VBE_INFO           (1 << 11)

// Multiboot info structure (passed to kernel)
typedef struct {
    uint32_t flags;

    // available memory from BIOS
    uint32_t mem_lower;
    uint32_t mem_upper;

    // "root" partition
    uint32_t boot_device;

    // kernel command line
    uint32_t cmdline;

    // boot modules
    uint32_t mods_count;
    uint32_t mods_addr;

    union {
        struct {
            uint32_t tabsize;
            uint32_t strsize;
            uint32_t addr;
            uint32_t reserved;
        } aout;

        struct {
            uint32_t num;
            uint32_t size;
            uint32_t addr;
            uint32_t shndx;
        } elf;
    } syms;

    // mmap_* are valid if flag bit 6 is set
    uint32_t mmap_length;
    uint32_t mmap_addr;

    // drives_* are valid if flag bit 7 is set
    uint32_t drives_length;
    uint32_t drives_addr;

    // ROM configuration table
    uint32_t config_table;

    // boot loader name
    uint32_t boot_loader_name;

    // APM table
    uint32_t apm_table;

    // VBE info
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;

    // Framebuffer info (linear graphics mode)
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
    union {
        struct {
            uint8_t red_field_position;
            uint8_t red_mask_size;
            uint8_t green_field_position;
            uint8_t green_mask_size;
            uint8_t blue_field_position;
            uint8_t blue_mask_size;
        } rgb;

        struct {
            uint32_t palette_addr;
            uint16_t palette_num_colors;
        } indexed;
    };
} __attribute__((packed)) multiboot_info_t;

#endif
