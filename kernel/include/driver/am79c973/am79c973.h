#ifndef __MERCURY__DRIVER__AM79C973__AM79C973_H
#define __MERCURY__DRIVER__AM79C973__AM79C973_H

#include <common/types.h>

void am79c973_init(uint8_t id);
void am79c973_enable();
void am79c973_disable();
void am79c973_send(char* buffer, size_t size);
void am79c973_receive();
uint64_t am79c973_get_mac_address();
uint32_t am79c973_get_ip_address();

#endif
