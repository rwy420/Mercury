#include <hardware/usb/xhci_controller.h>
#include <hardware/usb/usb_device.h>

void usb_device_init(USBDevice* device)
{
	xhci_init_control_endpoint(device);
}
