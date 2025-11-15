#ifndef __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H
#define __MERCURY__HARDWARE__USB__XHCI_CONTROLLER_H

#include <hardware/usb/usb_device.h>
#include <hardware/usb/xhci_structs.h>
#include <hardware/usb/xhci_device.h>
#include <hardware/pci.h>
#include <hardware/dma.h>
#include <hardware/usb/usb.h>

int xhci_take_ownership(DeviceDescriptor* device);
int xhci_create_controller(DeviceDescriptor* device);
int xhci_init_controller(DeviceDescriptor* device);
int xhci_init_ports();
int xchi_init_primary_int();
int xhci_reset_controller();
int xhci_init_scratchpad();
uint8_t xhci_init_device(uint32_t route, uint8_t depth, USB_SPEED speed, uint8_t parent_port_id);
int xhci_deinitialize_slot(uint8_t slot_id);
xHCITRB* xhci_send_command(xHCITRB* trb);
void xhci_advance_command_queue();
void xhci_init_control_endpoint(USBDevice* device);
void xhci_update_packet_size(USBDevice* device);

uint32_t xhci_send_request(USBDevice* device, USBRequestDescriptor* request, uint8_t* buffer);

void xhci_advance_endpoint_enqueue(volatile xHCIEndpoint* endpoint, int chain);

void xhci_handle_interrupt();
void xhci_updater_task();
void xhci_event_poll_task();

volatile uint32_t* xhci_doorbell_reg(uint32_t slot_id);

#endif
