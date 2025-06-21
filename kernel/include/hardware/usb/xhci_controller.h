#ifndef __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H
#define __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H

#define COMMAND_RING_TRB_COUNT 256
#define EVENT_RING_TRB_COUNT 252

#include <hardware/usb/xhci_structs.h>
#include <hardware/pci.h>
#include <hardware/dma.h>

typedef struct
{
	volatile xHCICapabilityRegs* capability_regs;
	volatile xHCIOperationalRegs* operational_regs;
	volatile xHCIRuntimeRegs* runtime_regs;
	xHCIPort ports[0x10];
	uint32_t bar0;
	uint8_t irq;
	DMARegion* dcbaa_region;
	DMARegion* command_ring_region;
	DMARegion* event_ring_region;
} xHCIController;

int xhci_take_ownership(DeviceDescriptor* device);
int xhci_create_controller(DeviceDescriptor* device);
int xhci_init_controller(DeviceDescriptor* device);
int xhci_init_ports();
int xchi_init_primary_int();
int xhci_reset_controller();

void xhci_handle_interrupt();

#endif
