#ifndef __MERCURY__DRIVER__RTL8139__RTL8139_H
#define __MERCURY__DRIVER__RTL8139__RTL8139_H

#include <common/types.h>
#include <driver/driver.h>

void rtl8139_init(Driver* self);
void rtl8139_enable();
void rtl8139_disable();
void rtl8139_send(char* buffer, size_t size);
void rtl8139_receive();

#endif
