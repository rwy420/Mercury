#ifndef __QSLBIC__QS_FAT16_H
#define __QSLBIC__QS_FAT16_H

__attribute__((weak)) int fat16_open(const char* filepath, char mode);
__attribute__((weak)) int fat16_close(unsigned char handle);
__attribute__((weak)) int fat16_read(unsigned char handle, void* buffer, unsigned char count);
__attribute__((weak)) int fat16_ls(unsigned int* index, char* filename, const char* dir_path); 

#endif
