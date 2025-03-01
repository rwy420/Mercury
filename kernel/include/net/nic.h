#ifndef __MERCURY__NET__NIC_H
#define __MERCURY__NET__NIC_H

#include <common/types.h>

typedef struct
{
	uint8_t mac[6];
} NIC;

NIC* create_nic();

#endif
