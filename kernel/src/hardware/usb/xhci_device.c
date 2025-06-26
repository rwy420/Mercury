#include <hardware/usb/xhci_structs.h>
#include <memory/heap.h>
#include <hardware/usb/xhci_device.h>

USBDevice* xhci_device_create(xHCIController* controller, USBInfo* info)
{
	USBDevice* device = kmalloc(sizeof(USBDevice));
	device->info = info;
	device->controller = controller;

	return device;
}
