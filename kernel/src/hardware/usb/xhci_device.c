#include "hardware/usb/xhci_structs.h"
#include <memory/heap.h>
#include <hardware/usb/xhci_device.h>

xHCIDevice* xhci_device_create(xHCIController* controller, USBInfo* info)
{
	xHCIDevice* device = kmalloc(sizeof(xHCIDevice));
	device->info = info;

	return device;
}
