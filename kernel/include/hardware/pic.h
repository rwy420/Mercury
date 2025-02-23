#ifndef __MERCURY__HARDWARE__PIC_H
#define __MERCURY__HARDWARE__PIC_H

#define PIC_1_COMMAND 0x20
#define PIC_1_DATA 0x21
#define PIC_2_COMMAND 0xA0
#define PIC_2_DATA 0x41
#define PIC_CONFIRM 0x20
#define PIC_ICW4_8086 0x01

#include <common/types.h>
#include <hardware/port.h>

void pic_remap(uint32_t offset1, uint32_t offset2);
void pic_confirm(uint32_t interrupt);

#endif
