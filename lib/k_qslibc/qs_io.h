#ifndef __QSLBIC__QS_IO_H
#define __QSLBIC__QS_IO_H

__attribute__((weak)) unsigned char inb(unsigned short port);
__attribute__((weak)) void outb(unsigned short port, unsigned char data);
__attribute__((weak)) void outb_slow(unsigned short port, unsigned char data);

__attribute__((weak)) unsigned short inw(unsigned short port);
__attribute__((weak)) void outw(unsigned short port, unsigned short data);

__attribute__((weak)) unsigned int inl(unsigned short port);
__attribute__((weak)) void outl(unsigned short port, unsigned int data);

#endif 
