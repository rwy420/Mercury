#ifndef __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H
#define __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H

#include <hardware/pci.h>

int xhci_take_ownership(DeviceDescriptor* device);
int xhci_create_controller(DeviceDescriptor* device);
int xhci_init_controller(DeviceDescriptor* device);

#endif
