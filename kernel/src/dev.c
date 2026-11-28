#include <dev.h>

Device* g_devices[MAX_DEV_NUM];
int device_num;

int device_register(Device *device)
{
	if(!device || !device->ops) return -1;

	g_devices[device_num] = device;

	return 0;
}
