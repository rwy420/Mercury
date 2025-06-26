#ifndef __MERCURY__HARDWARE__USB__USB_DEVICE_H
#define __MERCURY__HARDWARE__USB__USB_DEVICE_H

#include <hardware/dma.h>
#include <hardware/usb/usb.h>
#include <hardware/usb/xhci_structs.h>

void usb_device_init(xHCIController* controller, USBInfo* info);

#endif
