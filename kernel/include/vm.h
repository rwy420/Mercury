#ifndef __VM_H
#define __VM_H

#include <common/types.h>

static inline void dbg_putc(char c) { __asm__ volatile("outb %0, $0xE9" :: "a"(c)); }
void dbg_printf(char* str);
void dbg_print_hex(uint8_t h);
void dbg_print_hex32(uint32_t h);

#endif
