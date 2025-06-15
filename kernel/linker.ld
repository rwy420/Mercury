OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386:i386)
ENTRY(_start)

SECTIONS {
   /* The multiboot data and code will exist in low memory
      starting at 0x100000 */

   . = 0x00100000;
   .multiboot.data : {
       *(.multiboot.data)
   }

   .multiboot.text : {
       *(.multiboot.text)
   }

   /* The kernel will live at 3GB + 1MB in the virtual
      address space, which will be mapped to 1MB in the
      physical address space. */

   . += 0xC0000000;
   .text ALIGN(4096) : AT(ADDR(.text) - 0xC0000000) {
       *(.text)
   }

   .data ALIGN (4096) : AT(ADDR(.data) - 0xC0000000) {
       *(.data)
       *(.rodata*)
   }

   .bss ALIGN (4096) : AT(ADDR(.bss) - 0xC0000000) {
       *(COMMON)
       *(.bss*)
   }

   /DISCARD/ : {
       *(.eh_frame);
       *(.comment*);
   }

  ld_kernel_end = .;
  ld_kernel_start = .;
}

