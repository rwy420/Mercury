#ifndef __QS__HARDWARE__PIC_H
#define __QS__HARDWARE__PIC_H

#include <core/types.h>
#include <hardware/port.h>

void pic_remap(uint32_t offset1, uint32_t offset2);
void pic_confirm(uint32_t interrupt);

#endif
