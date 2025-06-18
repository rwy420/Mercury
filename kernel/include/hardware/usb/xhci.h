#ifndef __MERCURY__HARDWARE__USB__XHCI_H
#define __MERCURY__HARDWARE__USB__XHCI_H

#include <hardware/pci.h>

int xhci_take_ownership(DeviceDescriptor* device);

#endif
