#define STATUS_ROK (1<<0)
#define STATUS_TOK (1<<2)

#define MAC_0_5_PORT(base) base
#define MAC_5_6_PORT(base) base + 0x04
#define RBSTART_PORT(base) base + 0x30
#define COMMAND_PORT(base) base + 0x37
#define IMR_PORT(base) base + 0x3C
#define ISR_PORT(base) base + 0x3E
#define POWERUP_PORT(base) base + 0x52

#include <driver/rtl8139/rtl8139.h>
#include <hardware/port.h>
#include <hardware/pci.h>
#include <hardware/interrupts.h>
#include <common/screen.h>
#include <memory/heap.h>
#include <memory/common.h>
#include <net/nic.h>

static EthernetInterface* interface;
static uint32_t port_base;

uint8_t mac_address[6];
char rx_buffer[8192 + 16 + 1500];

static void rtl8139_reset()
{
    outb(COMMAND_PORT(port_base), 0x10);
    while((inb(COMMAND_PORT(port_base)) & 0x10) != 0);
}
static void rtl8139_handle_interrupt()
{
    printf("Got Interrupt\n");
    
    uint16_t status = inw(port_base + 0x3e);

    if(status & STATUS_TOK) {
        printf("Packet sent\n");
    }
    if (status & STATUS_ROK) {
        printf("Received packet\n");
        rtl8139_receive();
    }

    outw(port_base + 0x3E, 0x5);

    printf("Done\n");
}

void rtl8139_init(Driver* self)
{
	interface = (EthernetInterface*) self->driver_interface;
	NIC* nic = create_nic();

	interface->send = rtl8139_send;
	interface->receive = rtl8139_receive;

	port_base = self->device_descriptor->bars[0]->address & ~0x3;

	outb(POWERUP_PORT(port_base), 0x0);

	rtl8139_reset();

	void* buffer = kmalloc(8192 + 16 + 1500);
	memset(buffer, 0, 8192 + 16 + 1500);;
	outl(RBSTART_PORT(port_base), (uint32_t) buffer);

	outw(IMR_PORT(port_base), 0x05);

	outb(COMMAND_PORT(port_base), 0x0C);

	outl(port_base + 0x44, 0xF | (1 << 7));

	for (int i = 0; i < 6; i++)
	{
		nic->mac[i] = inb(port_base + i);
	}

	outb(POWERUP_PORT(port_base), 0x0);
	outb(POWERUP_PORT(port_base), 0x0);
	outb(POWERUP_PORT(port_base), 0x0);

	register_interrupt_handler(0x20 + self->device_descriptor->interrupt, (isr_t) rtl8139_handle_interrupt);
}

void rtl8139_enable()
{

}

void rtl8139_disable()
{

}

void rtl8139_send(char* buffer, size_t size)
{

}

void rtl8139_receive()
{

}
