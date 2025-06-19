#ifndef __MERCURY__HARDWARE__USB__USB_H
#define __MERCURY__HARDWARE__USB__USB_H

#include <hardware/pci.h>

void usb_init();
void usb_init_controller(DeviceDescriptor* device);

#endif
