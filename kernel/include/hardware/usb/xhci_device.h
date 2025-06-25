#ifndef __MERCURY__HARDWARE__USB__XHCI_DEVICE_H
#define __MERCURY__HARDWARE__USB__XHCI_DEVICE_H

#include <hardware/usb/xhci_structs.h>
#include <hardware/usb/usb.h>
#include <common/types.h>

xHCIDevice* xhci_device_create(xHCIController* controller, USBInfo* info);

#endif
