#define MAC_ADDRESS_0_PORT_OFFSET
#define MAC_ADDRESS_2_PORT_OFFSET
#define MAC_ADDRESS_4_PORT_OFFSET
#define REGISTER_DATA_PORT_OFFSET
#define REGISTER_ADDRESS_PORT_OFFSET
#define RESET_PORT_OFFSET
#define BUS_CONTROL_REGISTER_PORT_OFFSET

#include <driver/am79c973/am79c973.h>
#include <driver/driver.h>
#include <hardware/interrupts.h>
#include <common/screen.h>

uint8_t am79c973_id;
EthernetInterface* interface;

void am79c973_handle_interrupt(CPUState* cpu)
{

}

void am79c973_init(Driver* self)
{
	am79c973_id = self->id;
	interface = (EthernetInterface*) self->driver_interface;

	interface->send = am79c973_send;
	interface->receive = am79c973_receive;
	interface->get_mac_address = am79c973_get_mac_address;
	interface->get_ip_address = am79c973_get_ip_address;
}

void am79c973_enable()
{

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
