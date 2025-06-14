OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)
ENTRY(k_entry)

SECTIONS
{
    . = 0xC0100000;

    .text ALIGN(4K) : AT(0x00100000) {
        ld_kernel_start = .;
        *(.text*)
        *(.rodata*)
        *(.eh_frame*)
    }

    .data ALIGN(4K) : AT(LOADADDR(.text) + SIZEOF(.text)) {
        *(.data*)
    }

    .bss ALIGN(4K) : AT(LOADADDR(.data) + SIZEOF(.data)) {
        *(COMMON)
        *(.bss)
    }

    ld_kernel_end = .;

    /DISCARD/ : { *(.comment) *(.note*) }
}

