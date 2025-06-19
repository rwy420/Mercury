#ifndef __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H
#define __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H

#include <hardware/usb/xhci_structs.h>
#include <hardware/pci.h>

typedef struct
{
	xHCICapabilityRegs* capability_regs;
	xHCIOperationalRegs* operational_regs;
	xHCIRuntimeRegs* runtime_regs;
} xHCIController;

int xhci_take_ownership(DeviceDescriptor* device);
int xhci_create_controller(DeviceDescriptor* device);
int xhci_init_controller(DeviceDescriptor* device);

int xhci_reset_controller();

#endif
