#include <hardware/usb/usb.h>
#include <hardware/usb/usb_device.h>
#include <hardware/usb/xhci_controller.h>
#include <hardware/usb/xhci_structs.h>
#include <hardware/interrupts.h>
#include <hardware/dma.h>
#include <memory/heap.h>
#include <memory/paging.h>
#include <memory/frames.h>
#include <common/screen.h>
#include <memory/common.h>
#include <hardware/pit.h>
#include <multitasking.h>

xHCIController xhci_controller;
extern PageDirectory* g_kernel_pd;
extern volatile uint32_t g_ms_since_init;

int xhci_take_ownership(DeviceDescriptor* device)
{
	BAR* bar = device->bars[0];
	for(uint32_t i = bar->address; i < bar->address + bar->size * 2; i += PAGE_SIZE) map_page_pd_flags(g_kernel_pd, (void*) i, (void*) i, 
			PDE_PRESENT | PDE_RW | PDE_CACHE_DISABLE | PDE_WRITE_THROUGH);

	pci_enable_bus_mastering(device);
	pci_enable_memory(device);

	volatile xHCICapabilityRegs* capabilities = (volatile xHCICapabilityRegs*) bar->address;
	uint16_t extented_offset = capabilities->hccparams_1.xhci_extented_capability_pointer;

	if(extented_offset == 0x0)
	{
		return false;
	}

	uint32_t extented_address = bar->address + extented_offset * 4;

	while(true)
	{
		volatile xHCIExtentedCap* extented_cap = (volatile xHCIExtentedCap*) extented_address;

		if(extented_cap->capablity_id == USB_LEGACY_SUPPORT)
		{
			volatile xHCILegacyUSBSupportCap* legacy = (volatile xHCILegacyUSBSupportCap*) extented_address;
			if(!legacy->hc_bios_owned) return true;

			legacy->hc_os_owned = true;

			const uint32_t timeout = ms_since_init() + 1000;
			while(legacy->hc_bios_owned)
				if(ms_since_init() > timeout)
					return false;

			return true;
		}

		if(extented_cap->next_capability == 0) break;

		extented_address += extented_cap->next_capability;
	}

	return true;
}

int xhci_create_controller(DeviceDescriptor* device)
{
	// TODO

	if(xhci_init_controller(device))
		return true;
	else
		return false;
}

int xhci_init_controller(DeviceDescriptor* device)
{
	printf("<USB> Initializing xHCI Controller at ");
	print_hex(device->bus);
	printf(" ");
	print_hex(device->device_id);
	printf(" ");
	print_hex(device->function);
	printf("\n");

	BAR* bar = device->bars[0];

	xhci_controller.capability_regs = (volatile xHCICapabilityRegs*) bar->address;
	xhci_controller.operational_regs = (volatile xHCIOperationalRegs*) (bar->address + xhci_controller.capability_regs->cap_length);
	xhci_controller.runtime_regs = (volatile xHCIRuntimeRegs*) (bar->address + (xhci_controller.capability_regs->rstoff & ~0x1Fu));
	xhci_controller.irq = 0x20 + device->interrupt;
	xhci_controller.bar0 = bar->address;
	xhci_controller.command_cycle = 1;

	volatile xHCICapabilityRegs* capabilities = xhci_controller.capability_regs;

	if(bar->type != MM) return false;
	if(!xhci_reset_controller()) return false;

	printf("   Version: ");
	print_hex(capabilities->hci_version >> 8);
	printf(".");
	print_hex(capabilities->hci_version & 0xFF);
	printf("\n");

	printf("   Max slots ");
	print_hex32(+capabilities->hcsparams_1.max_slots);
	printf("\n   Max intrs ");
	print_hex32(+capabilities->hcsparams_1.max_ints);
	printf("\n   Max ports ");
	print_hex32(+capabilities->hcsparams_1.max_ports);
	printf("\n");

	if(!xhci_init_ports()) return false;

	volatile xHCIOperationalRegs* operational = xhci_controller.operational_regs;
	xhci_controller.dcbaa_region = dma_create(capabilities->hcsparams_1.max_slots * 8);
	memset((void*) xhci_controller.dcbaa_region->phys, 0, xhci_controller.dcbaa_region->size);
	uint64_t dcbaap_phys = xhci_controller.dcbaa_region->phys;
	operational->dcbaap_lo = dcbaap_phys & 0xFFFFFFFF;
	operational->dcbaap_hi = dcbaap_phys >> 32;

	xhci_controller.command_ring_region = dma_create(COMMAND_RING_TRB_COUNT * sizeof(xHCITRB));
	memset((void*) xhci_controller.command_ring_region->phys, 0, xhci_controller.command_ring_region->size);
	uint64_t command_ring_phsy = xhci_controller.command_ring_region->phys;
	operational->crcr_lo = command_ring_phsy | RING_CYCLE_STATE;
	operational->crcr_hi = command_ring_phsy >> 32;

	if(!xchi_init_primary_int()) return false;
	if(!xhci_init_scratchpad()) return false;

	operational->usb_cmd.run_stop = 1;
	while(operational->usbsts & HC_HALTED) continue;

	create_task(xhci_updater_task, true);
	create_task(xhci_event_poll_task, true);

	return true;
}

int xhci_init_ports()
{
	volatile xHCICapabilityRegs* capabilities  = xhci_controller.capability_regs;
	uint8_t max_ports = capabilities->hcsparams_1.max_ports;


	uint16_t extented_offset = capabilities->hccparams_1.xhci_extented_capability_pointer;
	uint32_t extented_address = xhci_controller.bar0 + extented_offset * 4;

	while(true)
	{
		const volatile xHCIExtentedCap* ext_cap = (volatile xHCIExtentedCap*) extented_address;

		if(ext_cap->capablity_id == SUPPORTED_PROTOCOL)
		{
			const volatile xHCISupportedProtocolCap* protocol = (volatile xHCISupportedProtocolCap*) ext_cap;
			const uint32_t protocol_name = ' BSU'; // "USB "

			if(protocol->name_string != protocol_name)
			{
				printf("<USB> Invalid protocol name\n");
				return false;
			}

			if(protocol->compatible_port_offset == 0 || protocol->compatible_port_offset + protocol->compatible_port_count -  1 > max_ports)
			{
				printf("<USB> Invalid ports\n");
				return false;
			}

			if(protocol->protocol_slot_type != 0)
			{
				printf("<USB> Invalid slot type\n");
				return false;
			}

			for(uint32_t i = 0; i < protocol->compatible_port_count; i++)
			{
				xHCIPort* port = &xhci_controller.ports[i];
				port->revision_major = protocol->major_version;
				port->revision_minor = protocol->minor_version;
			}
		}

		if(ext_cap->next_capability == 0) break;

		extented_address += ext_cap->next_capability * 4;
	}
	
	xhci_controller.operational_regs->config.max_device_slots_enabled = capabilities->hcsparams_1.max_slots;

	return true;
}

int xchi_init_primary_int()
{
	volatile xHCIRuntimeRegs* runtime = xhci_controller.runtime_regs;
	static uint32_t event_ring_table_offset  = EVENT_RING_TRB_COUNT * sizeof(xHCITRB);
	xhci_controller.event_ring_region = dma_create(EVENT_RING_TRB_COUNT * sizeof(xHCITRB) + sizeof(xHCIEventRingTableEntry));
	memset((void*) xhci_controller.event_ring_region->phys, 0, xhci_controller.event_ring_region->size);

	volatile xHCIEventRingTableEntry* event_ring_table_entry = (volatile xHCIEventRingTableEntry*) (xhci_controller.event_ring_region->phys + event_ring_table_offset);
	event_ring_table_entry->rsba = xhci_controller.event_ring_region->phys;
	event_ring_table_entry->rsz = EVENT_RING_TRB_COUNT;

	volatile xHCIInterruptRegs* primary_interrupter = (volatile xHCIInterruptRegs*) &runtime->irs[0];
	primary_interrupter->erstsz = (primary_interrupter->erstsz & 0xFFFF0000) | 1;
	primary_interrupter->erdp = xhci_controller.event_ring_region->phys | EVENT_HANDLER_BUSY;
	primary_interrupter->erstba = xhci_controller.event_ring_region->phys + event_ring_table_offset;

	volatile xHCIOperationalRegs* operational = xhci_controller.operational_regs;
	operational->usb_cmd.interrupter_enable = true;
	
	primary_interrupter->iman |= INTERRUPT_PEDNING | INTERRUPT_ENBLE;

	register_interrupt_handler(xhci_controller.irq, (isr_t) xhci_handle_interrupt);

	return true;
}

int xhci_init_scratchpad()
{
	volatile xHCICapabilityRegs* capabilities = xhci_controller.capability_regs;

	const uint32_t max_scratchpads = (capabilities->hcsparams_2.max_scratchpad_buffers_hi << 5) | capabilities->hcsparams_2.max_scratchpad_buffers_lo;
	if(max_scratchpads == 0) return true;

	printf("<CPU> xHCI Controller is using scratchpads\n");

	xhci_controller.scrachpad_buffer_region = dma_create(max_scratchpads * sizeof(uint64_t));
	uint64_t* scratchpad_buffer_array = (uint64_t*) xhci_controller.scrachpad_buffer_region->phys;
	xhci_controller.scratchpad_buffers = kmalloc(sizeof(uint32_t) * max_scratchpads);

	for(int i = 0; i < max_scratchpads; i++)
	{
		const uint32_t address = (uint32_t) alloc_frame();
		if(address == 0) return false;

		xhci_controller.scratchpad_buffers[i] =	address;
		scratchpad_buffer_array[i] = address;
	} 

	*(uint64_t*) xhci_controller.dcbaa_region->phys = xhci_controller.scrachpad_buffer_region->phys;

	return true;
}

int xhci_reset_controller()
{
	volatile xHCIOperationalRegs* operational = xhci_controller.operational_regs;
	const uint32_t timeout = ms_since_init() + 500;

	while(operational->usbsts & CONTROLLER_NOT_READY)
		if(ms_since_init() > timeout)
			return false;

	operational->usb_cmd.host_controller_reset = 1;
	while(operational->usb_cmd.host_controller_reset)
		if(ms_since_init() > timeout)
			return false;

	return true;
}

uint8_t xhci_init_device(uint32_t route, uint8_t depth, USB_SPEED speed, uint8_t parent_port_id)
{
	xHCITRB enable_slot;
	enable_slot.enable_slot_command.trb_type = ENABLE_SLOT_COMMAND;

	enable_slot.enable_slot_command.slot_type = 0;
	xHCITRB* result = xhci_send_command(&enable_slot);

	const uint8_t slot_id = result->command_completion_event.slot_id;

	if(slot_id == 0 || slot_id > xhci_controller.capability_regs->hcsparams_1.max_slots)
	{
		return 0;
	}


	xHCIPort* root_port = &xhci_controller.ports[(route & 0xF) - 1];

	USBInfo* info = kmalloc(sizeof(USBInfo));
	*info = (USBInfo) {parent_port_id, slot_id, depth, speed, route};
	USBDevice* device = xhci_device_create(&xhci_controller, info);

	xhci_controller.slots[slot_id - 1] = device;
	usb_device_init(device); 

	printf("<USB> Initialized USB ");
	print_hex(root_port->revision_major);
	printf(".");
	print_hex(root_port->revision_minor);
	printf(" device on slot ");
	print_hex(slot_id);
	printf("\n");

	return slot_id;
}

int xhci_deinitialize_slot(uint8_t slot_id)
{
	return true;
}


xHCITRB* xhci_send_command(xHCITRB* trb)
{
	volatile xHCIOperationalRegs* operational = xhci_controller.operational_regs;
	if(operational->usbsts & HC_HALTED)
	{
		return NULL_PTR;
	}

	volatile xHCITRB* command_trb = (volatile xHCITRB*) &((xHCITRB*) xhci_controller.command_ring_region->phys)[xhci_controller.command_dequeue];
	volatile xHCITRB* completion_trb = (volatile xHCITRB*) &(xhci_controller.command_completions[xhci_controller.command_dequeue]);

	command_trb->raw.dw0 = trb->raw.dw0;
	command_trb->raw.dw1 = trb->raw.dw1;
	command_trb->raw.dw2 = trb->raw.dw2;
	command_trb->raw.dw3 = trb->raw.dw3;
	command_trb->cycle = xhci_controller.command_cycle;

	completion_trb->raw.dw0 = 0;
	completion_trb->raw.dw1 = 0;
	completion_trb->raw.dw2 = 0;
	completion_trb->raw.dw3 = 0;

	xhci_advance_command_queue();


	// ISR not working yet, therefore this is actually being handled by a polling task TODO
	*xhci_doorbell_reg(0) = 0;

	uint32_t timeout = g_ms_since_init + 1000;
	volatile uint32_t* dw2 = &completion_trb->raw.dw2;
	while((*dw2 >> 24) == 0)
	{
		if(g_ms_since_init > timeout)
		{
			return NULL_PTR;
		}
	}

	if(completion_trb->command_completion_event.completion_code != 1)
	{
		return NULL_PTR;
	}

	return completion_trb;
}

void xhci_advance_command_queue()
{
	xhci_controller.command_dequeue++;
	if(xhci_controller.command_dequeue < COMMAND_RING_TRB_COUNT - 1) return;

	volatile xHCITRB* link_trb = (volatile xHCITRB*) &((xHCITRB*) xhci_controller.command_ring_region->phys)[xhci_controller.command_dequeue];
	link_trb->link_trb.trb_type = LINK;
	link_trb->link_trb.ring_segment_pointer = xhci_controller.command_ring_region->phys;
	link_trb->link_trb.interrupt_target = 0;
	link_trb->link_trb.cycle_bit = xhci_controller.command_cycle;
	link_trb->link_trb.toggle_cycle = 1;
	link_trb->link_trb.chain_bit = 0;
	link_trb->link_trb.interrupt_on_completion = 0;

	xhci_controller.command_dequeue = 0;
	xhci_controller.command_cycle = !xhci_controller.command_cycle;
}

void xhci_init_control_endpoint(USBDevice* device)
{
	const uint32_t context_size = device->controller->capability_regs->hccparams_1.context_size ? 64 : 32;
	device->controller->endpoints[0].cycle = 1;
	device->controller->endpoints[0].max_packet_size = 0;

	switch(device->info->speed)
	{
		case LOW_SPEED:
		case FULL_SPEED:
			device->endpoints[0].max_packet_size = 8;
			break;
		case HIGH_SPEED:
			device->endpoints[0].max_packet_size = 64;
			break;
		case SUPER_SPEED:
			device->endpoints[0].max_packet_size = 512;
			break;
		default:
			break;
	}

	device->input_context = dma_create(33 * context_size);
	memset((void*) device->input_context->phys, 0, device->input_context->size);

	device->output_context = dma_create(32 * context_size);
	memset((void*) device->output_context->phys, 0, device->output_context->size);

	device->endpoints[0].transfer_ring = dma_create(TRANSFER_RING_TRB_COUNT * sizeof(xHCITRB));
	memset((void*) device->endpoints[0].transfer_ring->phys, 0, device->endpoints[0].transfer_ring->size);

	volatile xHCIInputControlContext* input_control_context = (volatile xHCIInputControlContext*) device->input_context->phys + 0 * context_size;
	volatile xHCISlotContext* slot_context = (volatile xHCISlotContext*) device->input_context->phys + 1 * context_size;
	volatile xHCIEndpointContext* endpoint0_context = (volatile xHCIEndpointContext*) device->input_context->phys + 2 * context_size;

	input_control_context->add_context_flags = (1 << 1) | (1 << 0);

	slot_context->route = device->info->route >> 4;
	slot_context->root_hub_port_number = device->info->route & 0x0F;
	slot_context->context_entries = 1;
	slot_context->interrupter_target = 0;
	slot_context->speed = usb_class_to_speed(device->info->speed);

	endpoint0_context->endpoint_type = CONTROL;
	endpoint0_context->max_packet_size = device->endpoints[0].max_packet_size;
	endpoint0_context->max_burst_size = 0;
	endpoint0_context->interval = 0;
	endpoint0_context->tr_dequeue_pointer = device->endpoints[0].transfer_ring->phys | 1;
	endpoint0_context->max_primary_streams = 0;
	endpoint0_context->error_count = 3;

	volatile uint32_t dcbaa_reg = ((uint32_t*) device->controller->dcbaa_region->phys)[device->info->slot_id];
	dcbaa_reg = device->output_context->phys;

	for(int i = 0; i < 2; i++)
	{
		xHCITRB address_device;
		address_device.address_device_command.trb_type = ADDRESS_DEVICE_COMMAND;
		address_device.address_device_command.input_context_pointer = device->input_context->phys;
		address_device.address_device_command.block_set_address_request = (i == 0);
		address_device.address_device_command.slot_id = device->info->slot_id;

		xhci_send_command(&address_device);
	}


}

void xhci_update_packet_size(USBDevice* device)
{
	uint8_t buffer[8];

	USBRequestDescriptor request = {DEVICE_TO_HOST | STANDART | DEVICE, GET_DESCRIPTOR, DEVICE_DESCRIPTOR << 8, 0, 8};
}

void xhci_handle_interrupt()
{
	printf("xHCI Interrupt\n");
}

void xhci_updater_task()
{
	int ports_changed = true;
	int ports_initialized = false;
	uint32_t last_port_update = g_ms_since_init;

	while(1)
	{
		{
			int expected = true;

			while(!atomic_compare_exchange(&ports_changed, &expected, false))
			{
				if(!ports_initialized && g_ms_since_init - last_port_update >= 100)
				{
					printf("INIT\n");
					ports_initialized = true;
				}

				uint32_t timeout = g_ms_since_init + 100;
	
				while(g_ms_since_init < timeout);

				expected = true;
			}
		}

		last_port_update = g_ms_since_init;

		for(int i = 0; i < 0x10; i++)
		{
			xHCIPort* port = &xhci_controller.ports[i];
			if(port->revision_major == 0) continue;

			volatile xHCIPortRegs* op_port = (volatile xHCIPortRegs*) &xhci_controller.operational_regs->ports[i];
			if(!(op_port->portsc & PP)) continue;

			const int reset_change = op_port->portsc & PRC;
			const int connection_change = op_port->portsc & CSC;
			const int port_enabled = op_port->portsc & PED;
			op_port->portsc = CSC | PRC | PP;

			if(!(op_port->portsc & CCS))
			{
				if(port->slot_id != 0)
				{
					xhci_deinitialize_slot(port->slot_id);
					port->slot_id = 0;
				}

				continue;
			}

			switch(port->revision_major)
			{
				case 2:
					if(port_enabled && reset_change) break;
					if(connection_change) op_port->portsc = PR | PP;

					continue;

				case 3:
					if(!connection_change || !port_enabled) continue;

					break;

				default:
					continue;
			}

			const uint8_t speed_id = (op_port->portsc >> PORT_SPEED_SHIFT) & PORT_SPEED_MASK;
			
			port->slot_id = xhci_init_device(i + 1, 0, usb_speed_to_class(speed_id), 0);
		}
	}
}

void xhci_event_poll_task()
{
	while(1)
	{
		volatile xHCITRB* ring = (volatile xHCITRB*) xhci_controller.event_ring_region->phys;
		volatile xHCIInterruptRegs* intr = &xhci_controller.runtime_regs->irs[0];

		static size_t dequeue_index = 0;
		static int cycle_state = true;

		volatile xHCITRB* trb = &ring[dequeue_index];

		if((trb->raw.dw3 & 1) != cycle_state)
		{
			// yield
			asm("int $0x20");
			continue;
		}

		switch(trb->trb_type)
		{
			case TRANSFER_EVENT:
			{
				break;
			}

			case COMMAND_COMPLETION_EVENT:
			{
				const uint32_t trb_index = (trb->command_completion_event.command_trb_pointer - xhci_controller.command_ring_region->phys) / sizeof(xHCITRB);

				volatile xHCITRB* completion_trb = (volatile xHCITRB*) &xhci_controller.command_completions[trb_index];
				completion_trb->raw.dw0 = trb->raw.dw0;
				completion_trb->raw.dw1 = trb->raw.dw1;
				completion_trb->raw.dw2 = trb->raw.dw2;
				completion_trb->raw.dw3 = trb->raw.dw3;

				break;
			}

			default:
				printf("<USB> Unhandled event: 0x");
				print_hex(trb->trb_type);
				printf("\n");
		}

		dequeue_index++;
		if(dequeue_index >= EVENT_RING_TRB_COUNT)
		{
			dequeue_index = 0;
			cycle_state ^= 1; 
		}

		intr->erdp = xhci_controller.event_ring_region->phys + dequeue_index * sizeof(xHCITRB);
		intr->iman |= INTERRUPT_PEDNING;
	}
}

volatile uint32_t* xhci_doorbell_reg(uint32_t slot_id)
{
	return (volatile uint32_t*) &((uint32_t*) (xhci_controller.bar0 + xhci_controller.capability_regs->dboff))[slot_id];
}
