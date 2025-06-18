#include <hardware/usb/xhci.h>
#include <memory/paging.h>
#include <common/screen.h>

void xhci_take_ownership(DeviceDescriptor* device)
{
	print_hex32(device->bars[0]->size);
	printf("\n");
}
