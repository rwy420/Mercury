#define MAC_ADDRESS_0_PORT_OFFSET 0x00
#define MAC_ADDRESS_2_PORT_OFFSET 0x02
#define MAC_ADDRESS_4_PORT_OFFSET 0x04
#define REGISTER_DATA_PORT_OFFSET 0x10
#define REGISTER_ADDRESS_PORT_OFFSET 0x12
#define RESET_PORT_OFFSET 0x14
#define BUS_CONTROL_REGISTER_PORT_OFFSET 0x16

#include <driver/am79c973/am79c973.h>
#include <driver/driver.h>
#include <hardware/interrupts.h>
#include <hardware/port.h>
#include <common/screen.h>
#include <memory/common.h>

EthernetInterface* interface;

uint32_t port_base;

InitBlock init_block;
BufferDescriptor* send_buffer_descriptor;
BufferDescriptor* receive_buffer_descriptor;
char send_buffer_descriptor_memory[2 * 1024 + 15];
char send_buffers[2 * 1024 + 15][8];
uint8_t current_send_buffer;

char receive_buffer_descriptor_memory[2 * 1024 + 15];
char receive_buffers[2 * 1024 + 15][8];
uint8_t current_receive_buffer;

void am79c973_handle_interrupt(CPUState* cpu)
{
	printf("AM79C973 INTERRUPT\n");

	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x00);
	uint32_t tmp = inw(port_base + REGISTER_DATA_PORT_OFFSET);

	if((tmp & 0x8000) == 0x8000) printf("AM79C973 ERROR\n");
	if((tmp & 0x2000) == 0x2000) printf("AM79C973 COLLISION ERROR\n");
	if((tmp & 0x1000) == 0x1000) printf("AM79C973 MISSED FRAME\n");
	if((tmp & 0x800) == 0x800) printf("AM79C973 MEMORY ERROR\n");
	if((tmp & 0x400) == 0x400) printf("AM79C973 DATA RECEIVED\n");
	if((tmp & 0x200) == 0x200) printf("AM79C973 DATA SENT\n");
	if((tmp & 0x100) == 0x100) printf("AM79C973 INIT DONE\n");

	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x00);
	outw(port_base + REGISTER_DATA_PORT_OFFSET, tmp);	
}

void am79c973_init(Driver* self)
{
	print_uint8_t(self->device_descriptor->interrupt + 0x20);
	register_interrupt_handler(self->device_descriptor->interrupt + 0x20, am79c973_handle_interrupt);

	interface = (EthernetInterface*) self->driver_interface;
	port_base = self->device_descriptor->port_base[0];

	interface->send = am79c973_send;
	interface->receive = am79c973_receive;
	interface->get_mac_address = am79c973_get_mac_address;
	interface->get_ip_address = am79c973_get_ip_address;


	current_send_buffer = 0;
	current_receive_buffer = 0;

	uint64_t MAC0 = inw(port_base + MAC_ADDRESS_0_PORT_OFFSET) % 256;
	uint64_t MAC1 = inw(port_base + MAC_ADDRESS_0_PORT_OFFSET) / 256;
	uint64_t MAC2 = inw(port_base + MAC_ADDRESS_2_PORT_OFFSET) % 256;
	uint64_t MAC3 = inw(port_base + MAC_ADDRESS_2_PORT_OFFSET) / 256;
	uint64_t MAC4 = inw(port_base + MAC_ADDRESS_4_PORT_OFFSET) % 256;
	uint64_t MAC5 = inw(port_base + MAC_ADDRESS_4_PORT_OFFSET) / 256;

	uint64_t MAC = MAC5 << 40 | MAC4 << 32 | MAC3 << 24 | MAC2 << 16 | MAC1 << 8 | MAC0;

	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x14);
	outw(port_base + BUS_CONTROL_REGISTER_PORT_OFFSET, 0x102);

	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x00);
	outw(port_base + REGISTER_DATA_PORT_OFFSET, 0x04);

	init_block.mode = 0x00;
	init_block.reserved1 = 0x00;
	init_block.num_send_buffers = 0x03;
	init_block.reserved2 = 0x00;
	init_block.num_receive_buffers = 0x03;
	init_block.physical_address = MAC;
	init_block.reserved3 = 0x00;
	init_block.logical_address = 0x00;

	send_buffer_descriptor = (BufferDescriptor*)((((uint32_t) &send_buffer_descriptor_memory[0]) + 0xF) & ~((uint32_t) 0xF));
	init_block.send_buffer_descriptor_address = (uint32_t) send_buffer_descriptor;

	receive_buffer_descriptor = (BufferDescriptor*)((((uint32_t) &receive_buffer_descriptor_memory[0]) + 0xF) & ~((uint32_t) 0xF));
	init_block.receive_buffer_descriptor_address = (uint32_t) receive_buffer_descriptor;

	for(int i = 0; i < 8; i++)
	{
		send_buffer_descriptor[i].address = (((uint32_t) &send_buffers[i]) + 0xF) & ~(uint32_t) 0x0F;
		send_buffer_descriptor[i].flags = 0x7FF | 0xF000;
		send_buffer_descriptor[i].flags2 = 0x00;
		send_buffer_descriptor[i].available = 0x00;

		receive_buffer_descriptor[i].address = (((uint32_t) &receive_buffers[i]) + 0xF) & ~(uint32_t) 0x0F;
		receive_buffer_descriptor[i].flags = 0xF7FF | 0x80000000;
		receive_buffer_descriptor[i].flags2 = 0x00;
		receive_buffer_descriptor[i].available = 0x00;
	}

	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x01);
	outw(port_base + REGISTER_DATA_PORT_OFFSET, (uint32_t)(&init_block) & 0xFFFF);
	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x02);
	outw(port_base + REGISTER_DATA_PORT_OFFSET, ((uint32_t)(&init_block) >> 16) & 0xFFFF);
}

void am79c973_enable()
{
	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x00);
	outw(port_base + REGISTER_DATA_PORT_OFFSET, 0x41);

	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x04);
	uint32_t tmp = inw(port_base + REGISTER_DATA_PORT_OFFSET);
	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x04);
	outw(port_base + REGISTER_DATA_PORT_OFFSET, tmp | 0xC00);

	outw(port_base + REGISTER_ADDRESS_PORT_OFFSET, 0x00);
	outw(port_base + REGISTER_DATA_PORT_OFFSET, 0x42);
}

void am79c973_disable()
{

}

void am79c973_send(char* buffer, size_t size)
{

}

void am79c973_receive()
{

}

uint64_t am79c973_get_mac_address()
{
	return 0;
}

uint32_t am79c973_get_ip_address()
{
	return 0;
}
