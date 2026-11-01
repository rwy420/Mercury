#ifndef __MERCURY__FS__USTAR__USTAR_H
#define __MERCURY__FS__USTAR__USTAR_H

#include <common/types.h>

typedef struct
{
	char name[100];
	uint8_t mode[8];
	uint8_t owner_id[8];
	uint8_t group_id[8];
	uint8_t size_in_bytes[12];
	uint8_t last_modified[12];
	uint8_t checksum[8];
	uint8_t type_flag;
	char name_linked_file[100];
	char indicator[6];
	uint16_t version;
	char owner_name[32];
	char group_name[32];
	uint8_t dev_major_number[8];
	uint8_t dev_minor_number[8];
	char filename_prefix[155];
} UStarNode;



#endif
