#ifndef __MERCURY__HARDWARE__PIC_H
#define __MERCURY__HARDWARE__PIC_H

#include <common/types.h>
#include <hardware/port.h>

void pic_remap(uint32_t offset1, uint32_t offset2);
void pic_confirm(uint32_t interrupt);

#endif
