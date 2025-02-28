#include <driver/am79c973/am79c973.h>
#include <driver/driver.h>
#include <common/screen.h>

uint8_t am79c973_id;
EthernetInterface* interface;

void am79c973_handle_interrupt(void)
{

}

void am79c973_init(uint8_t id)
{
	am79c973_id = id;
	interface = get_interface(am79c973_id);

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
