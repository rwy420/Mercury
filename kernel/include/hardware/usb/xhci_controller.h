#ifndef __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H
#define __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H

#define COMMAND_RING_TRB_COUNT 256
#define EVENT_RING_TRB_COUNT 252

#include <hardware/usb/xhci_structs.h>
#include <hardware/pci.h>
#include <hardware/dma.h>
#include <hardware/usb/usb.h>

typedef struct
{
	volatile xHCICapabilityRegs* capability_regs;
	volatile xHCIOperationalRegs* operational_regs;
	volatile xHCIRuntimeRegs* runtime_regs;
	xHCIPort ports[0x10];
	uint32_t command_queue;
	xHCITRB command_completions[COMMAND_RING_TRB_COUNT];
	int command_cycle;
	uint32_t bar0;
	uint8_t irq;
	DMARegion* dcbaa_region;
	DMARegion* command_ring_region;
	DMARegion* event_ring_region;
	DMARegion* scrachpad_buffer_region;
	uint32_t* scratchpad_buffers;
} xHCIController;

int xhci_take_ownership(DeviceDescriptor* device);
int xhci_create_controller(DeviceDescriptor* device);
int xhci_init_controller(DeviceDescriptor* device);
int xhci_init_ports();
int xchi_init_primary_int();
int xhci_reset_controller();
int xhci_init_scratchpad();
uint8_t xhci_initialize_device(uint32_t route, uint8_t depth, USB_SPEED speed, uint8_t parent_port_id);
int xhci_deinitialize_slot(uint8_t slot_id);
xHCITRB xhci_send_command(xHCITRB* trb);
void xhci_advance_command_queue();

void xhci_handle_interrupt();
void xhci_updater_task();
void xhci_event_poll_task();

volatile uint32_t* xhci_doorbell_reg(uint32_t slot_id);

#endif
