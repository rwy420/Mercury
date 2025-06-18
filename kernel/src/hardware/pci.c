#include <hardware/pci.h>
#include <memory/heap.h>
#include <memory/paging.h>
#include <memory/common.h>
#include <driver/sata/sata.h>
#include <driver/driver.h>
#include <common/screen.h>
#include <driver/am79c973/am79c973.h>
#include <driver/rtl8139/rtl8139.h>

#define DATA_PORT 0xCFC
#define COMMAND_PORT 0xCF8

DeviceDescriptor* g_pci_devices;
uint8_t g_pci_num_devices;

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

uint32_t pci_read32(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset)
{
    uint32_t address = (1U << 31)
                     | ((uint32_t)bus << 16)
                     | ((uint32_t)device << 11)
                     | ((uint32_t)function << 8)
                     | (offset & 0xFC);

    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

uint16_t pci_read16(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset)
{
    uint32_t data = pci_read32(bus, device, function, offset);
    return (data >> ((offset & 2) * 8)) & 0xFFFF;
}

uint8_t pci_read8(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset)
{
    uint32_t data = pci_read32(bus, device, function, offset);
    return (data >> ((offset & 3) * 8)) & 0xFF;
}


void pci_write(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset, uint16_t value)
{
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (offset & 0xFC);
	outl(COMMAND_PORT, id);
	outl(DATA_PORT, value);
}

void pci_enumerate_devices(int debug)
{
	g_pci_devices = kmalloc(sizeof(DeviceDescriptor) * 256);
	memset(g_pci_devices, 0, sizeof(DeviceDescriptor) * 256);
	g_pci_num_devices = 0;

	for(int bus = 0; bus < 8; bus++)
	{
		for(int device = 0; device < 32; device++)
		{
			int function_count = has_functions(bus, device) ? 8 : 1; 
			for(int function = 0; function < function_count; function++)
			{
				g_pci_num_devices++;
				DeviceDescriptor* device_descriptor = &g_pci_devices[g_pci_num_devices];
				

				device_descriptor->vendor_id = pci_get_vendor_id(bus, device, function);
				device_descriptor->device_id = pci_get_device_id(bus, device, function);

				if(device_descriptor->vendor_id == 0x0000 || device_descriptor->vendor_id == 0xFFFF)
				{
					memset(device_descriptor, 0, sizeof(DeviceDescriptor));
					g_pci_num_devices--;
					continue;
				}

				device_descriptor->interrupt = pci_get_interrupt(bus, device, function);
				device_descriptor->prog_if = pci_get_prog_if(bus, device, function);
				device_descriptor->class_id = pci_get_class_id(bus, device, function);
				device_descriptor->subclass_id = pci_get_subclass_id(bus, device, function);



				for(uint8_t bar_idx = 0; bar_idx < 6; bar_idx++)
				{
					BAR* bar = pci_get_bar(bus, device, function, bar_idx);
					device_descriptor->port_base[bar_idx] = bar->address;

					kfree(bar);
				}

				if(debug) 
				{
					print_hex(bus & 0xFF);
					printf(" ");
					print_hex(device & 0xFF);
					printf(" ");
					print_hex(function & 0xFF);
					printf(" ");
					/*print_hex((device_descriptor->vendor_id & 0xFF00) >> 8);
					print_hex(device_descriptor->vendor_id & 0xFF);
					printf(" ");
					print_hex((device_descriptor->device_id & 0xFF00) >> 8);
					print_hex(device_descriptor->device_id & 0xFF);
					printf(" ");
					print_hex32(device_descriptor->port_base[0]);
					printf(" ");*/
					print_hex(device_descriptor->class_id);
					printf(" ");
					print_hex(device_descriptor->subclass_id);
					printf(" ");
					print_hex((device_descriptor->prog_if & 0xFF00) >> 8);
					print_hex(device_descriptor->prog_if & 0xFF);
					printf("\n");
				}

				//get_driver(device_descriptor);
			}
		}
	}
}

void get_driver(DeviceDescriptor* device_descriptor)
{
	switch(device_descriptor->vendor_id)
	{
		case 0x8086:
			switch (device_descriptor->vendor_id)
			{
				case 0x2829:
					for(int page = 0; page < 6; page++)
					{
						//TODO TEST THIS
						//map_page((void*)(device_descriptor->port_base[4] + (4096 * page)), (void*)(device_descriptor->port_base[4] + (4096 * page)), 
						//		PTE_RW);
					}
					init_sata(device_descriptor->port_base[4]); // BAR5
					break;
			}
			break;
		case 0x1022:
			switch(device_descriptor->device_id)
			{
				case 0x2000:
					create_driver("AMD-AM79C973", ETHERNET, am79c973_init, am79c973_enable, am79c973_disable, device_descriptor);
					break;			
			}
			break;
		case 0x10EC:
			switch(device_descriptor->device_id)
			{
				case 0x8139:
					create_driver("RTL-8139", ETHERNET, rtl8139_init, rtl8139_enable, rtl8139_disable, device_descriptor);
					break;
			} 
	}
}

uint16_t pci_get_vendor_id(uint16_t bus, uint16_t device, uint16_t function)
{
	return pci_read16(bus, device, function, 0x00);
}

uint16_t pci_get_device_id(uint16_t bus, uint16_t device, uint16_t function)
{	
	return pci_read16(bus, device, function, 0x02);
}

uint16_t pci_get_class_id(uint16_t bus, uint16_t device, uint16_t function)
{
	return pci_read8(bus, device, function, 0xB);
}

uint16_t pci_get_subclass_id(uint16_t bus, uint16_t device, uint16_t function)
{
	return pci_read16(bus, device, function, 0xA);
}

uint16_t pci_get_interrupt(uint16_t bus, uint16_t device, uint16_t function)
{
	return pci_read16(bus, device, function, 0x3C);
}

uint16_t pci_get_prog_if(uint16_t bus, uint16_t device, uint16_t function)
{
	return pci_read8(bus, device, function, 0x9);
}

BAR* pci_get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
	BAR* result = kmalloc(sizeof(BAR));

	uint32_t header_type = pci_read32(bus, device, function, 0x0E) & 0x7F;
	int max_bars = 6 - (4 * header_type);

	if(bar >= max_bars) return result;

	uint32_t bar_value = pci_read32(bus, device, function, 0x10 + 4 * bar);
	result->type = (bar_value & 0x1) ? IO : MM;
	uint32_t temp;

	if(result->type == MM)
	{
		result->address = bar_value & 0xFFFFFFF0;
		result->prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
	}
	else
	{
		result->address = bar_value & ~0x3;
		result->prefetchable = false;
	}

	return result;
}

int has_functions(uint16_t bus, uint16_t device)
{
	return pci_read16(bus, device, 0, 0x0E) & (1 << 7);
}
