#include "core/screen.h"
#include "driver/driver.h"
#include <hardware/pci.h>

#define DATA_PORT 0xCFC
#define COMMAND_PORT 0xCF8

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

void pci_enumerate_devices()
{
	for(uint32_t bus = 0; bus < 256; bus++)
	{
		for(uint32_t slot = 0; slot < 32; slot++)
		{
			for(uint32_t function = 0; function < 8; function++)
			{
				uint16_t vendor_id = pci_get_vendor_id(bus, slot, function);
				if(vendor_id == 0xFFFF) continue;

				uint16_t device_id = pci_get_device_id(bus, slot, function);

				struct Driver* driver = get_driver(vendor_id, device_id);

				if(driver != 0) add_driver(driver);
			}
		}
	}
}

struct Driver* get_driver(uint16_t vendor, uint16_t device)
{
	switch(vendor)
	{
		case 0x1022: // AMD
		{
			switch(device)
			{
				case 0x2000: //79c970
					break;
			}
			break;
		}

		case 0x8086: // Intel
		{

		}
	}
	
	return 0;
}

uint16_t pci_get_vendor_id(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t result = pci_read(bus, device, function, 0);
	return result;
}

uint16_t pci_get_device_id(uint16_t bus, uint16_t device, uint16_t function)
{	
	uint32_t result = pci_read(bus, device, function, 2);
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

/*struct PCIDeviceDescriptor* pci_get_device_descriptor(uint16_t bus, uint16_t device, uint16_t function)
{
	struct PCIDeviceDescriptor* result;

	result->bus = bus;
	result->device = device;
	result->function = function;

	result->vendor_id = pci_read(bus, device, function, 0x00);
	result->device_id = pci_read(bus, device, function, 0x02);

	result->class_id = pci_read(bus, device, function, 0x0B);
	result->subclass_id = pci_read(bus, device, function, 0x0A);
	result->interface_id = pci_read(bus, device, function, 0x09);
	
	result->revision = pci_read(bus, device, function, 0x08);
	result->interrupt = pci_read(bus, device, function, 0x3C);

	return result;
}

struct BAR* pci_get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
	struct BAR* result;

	uint32_t header_type = pci_read(bus, device, function, 0x0E) & 0x7F;
	int max_bars = 6 - (4 * header_type);

	if(bar >= max_bars) return result;

	uint32_t bar_value = pci_read(bus, device, function, 0x10 + 4 * bar);
	result->type = (bar_value & 0x1) ? IO : MM;
	uint32_t temp;

	if(result->type == MM)
	{
		switch ((bar_value >> 1) & 0x3)
		{
			case 0:
			{
				// 32 Bit mode
				break;
			}
			case 1:
			{
				// 20 Bit mode
				break;
			}
			case 2:
			{
				// 64 Bit mode
				break;
			}
		}
		result->prefetchable = ((bar_value >> 3) & 0x1) == 0x1;
	}
	else
	{
		// IO
		result->address = (uint8_t*) (bar_value & ~0x3);
		result->prefetchable = false;
	}

	return result;
}*/
