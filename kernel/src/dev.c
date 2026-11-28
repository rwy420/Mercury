#include <dev.h>

Device* g_devices[MAX_DEV_NUM];
int device_num;
int drive_num;

int device_register(Device *device)
{
	if(!device || !device->ops) return -1;

	g_devices[device_num] = device;

	return 0;
}

void gen_drive_name(char* name)
{
	name[0] = 's';
	name[1] = 'd';
	name[2] = 'a' + drive_num++;
	name[3] = '\n';
}
