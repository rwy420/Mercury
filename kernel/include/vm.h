#ifndef __VM_H
#define __VM_H

static inline void dbg_putc(char c) { __asm__ volatile("outb %0, $0xE9" :: "a"(c)); }
void dbg_printf(char* str);

#endif
