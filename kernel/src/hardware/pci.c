#include <hardware/pci.h>
#include <memory/mem_manager.h>
#include <memory/paging.h>
#include <driver/sata/sata.h>
#include <driver/driver.h>
#include <common/screen.h>
#include <driver/am79c973/am79c973.h>

#define DATA_PORT 0xCFC
#define COMMAND_PORT 0xCF8

DeviceDescriptor devices[MAX_DEVICES];
uint8_t num_devices;

uint32_t pci_read(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset)
{
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (offset & 0xFC);
	outl(COMMAND_PORT, id);
	uint32_t result = inl(DATA_PORT);
	return result >> (8 * (offset % 4));
}

void pci_write(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset, uint32_t value)
{
	uint32_t id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (offset & 0xFC);
	outl(COMMAND_PORT, id);
	outl(DATA_PORT, value);
}

void pci_enumerate_devices(bool debug)
{
	num_devices = 0;

	for(int bus = 0; bus < 8; bus++)
	{
		for(int device = 0; device < 32; device++)
		{
			int function_count = has_functions(bus, device) ? 8 : 1; 
			for(int function = 0; function < function_count; function++)
			{
				DeviceDescriptor* device_descriptor = &devices[num_devices];
				num_devices++;
				

				device_descriptor->vendor_id = pci_get_vendor_id(bus, device, function);
				device_descriptor->device_id = pci_get_device_id(bus, device, function);
				device_descriptor->interrupt = pci_get_interrupt(bus, device, function);

				if(device_descriptor->vendor_id == 0x0000 || device_descriptor->vendor_id == 0xFFFF) continue;

				for(uint8_t bar_idx = 0; bar_idx < 6; bar_idx++)
				{
					BAR* bar = pci_get_bar(bus, device, function, bar_idx);
					/*if(bar->address && (bar->type == IO)) 
					{
						port_base = bar->address;
					}
					
					if(bar->type == MM)
					{
						if(bar->address && bar_idx == 5)
						{
							port_base = bar->address;
						}
					}*/

					device_descriptor->port_base[bar_idx] = bar->address;

					free(bar);
				}

				if(debug) 
				{
					print_hex(bus & 0xFF);
					printf(" ");
					print_hex(device & 0xFF);
					printf(" ");
					print_hex(function & 0xFF);
					printf(" ");
					print_hex((device_descriptor->vendor_id & 0xFF00) >> 8);
					print_hex(device_descriptor->vendor_id & 0xFF);
					printf(" ");
					print_hex((device_descriptor->device_id & 0xFF00) >> 8);
					print_hex(device_descriptor->device_id & 0xFF);
					printf(" ");
					print_hex32(device_descriptor->port_base[0]);
					printf("\n");
				}

				get_driver(device_descriptor);
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
						map_page((void*)(device_descriptor->port_base[4] + (4096 * page)), (void*)(device_descriptor->port_base[4] + (4096 * page)), 
								PTE_RW);
					}
					init_sata(device_descriptor->port_base[4]); // BAR5
					break;
			}
			break;
		case 0x1022:
			switch(device_descriptor->device_id)
			{
				case 0x2000:
				{
					create_driver("AMD-AM79C973", ETHERNET, am79c973_init, am79c973_enable, am79c973_disable, device_descriptor);
					break;
				}
			}
		break;
	}
}

uint16_t pci_get_vendor_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t result = pci_read(bus, device, function, 0x00);
	return result;
}

uint16_t pci_get_device_id(uint16_t bus, uint16_t device, uint16_t function)
{	
	uint32_t result = pci_read(bus, device, function, 0x02);
	return result;
}

uint16_t pci_get_class_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t result = pci_read(bus, device, function, 0xA);
	return (result & ~0x00FF) >> 8;
}

uint16_t pci_get_subclass_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t result = pci_read(bus, device, function, 0xA);
	return (result & ~0xFF00);
}

uint16_t pci_get_interrupt(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t result = pci_read(bus, device, function, 0x3C);
	return (result & ~0xFF00);
}

BAR* pci_get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
	BAR* result = malloc(sizeof(BAR));

	uint32_t header_type = pci_read(bus, device, function, 0x0E) & 0x7F;
	int max_bars = 6 - (4 * header_type);

	if(bar >= max_bars) return result;

	uint32_t bar_value = pci_read(bus, device, function, 0x10 + 4 * bar);
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

bool has_functions(uint16_t bus, uint16_t device)
{
	return pci_read(bus, device, 0, 0x0E) & (1 << 7);
}
