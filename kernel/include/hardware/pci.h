#ifndef __MERCURY__HARDWARE__PCI_H
#define __MERCURY__HARDWARE__PCI_H

#define MAX_DEVICES 0xFF

#include <hardware/port.h>
#include <common/types.h>

typedef enum 
{
	MM = 0,
	IO = 1
} BAR_TYPE;

typedef struct
{
	uint16_t vendor_id, device_id;
	uint16_t bus, function, class_id, subclass_id;
	uint32_t interrupt;
	uint32_t port_base[6];
} DeviceDescriptor;

typedef struct
{
	bool prefetchable;
	uint32_t address;
	uint32_t size;
 	BAR_TYPE type;
} __attribute__((packed)) BAR;

uint32_t pci_read(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset);
void pci_write(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset, uint32_t value);

void pci_enumerate_devices(bool debug);
void get_driver(DeviceDescriptor* device_descriptor);

uint16_t pci_get_vendor_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_device_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_class_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_subclass_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_interrupt(uint16_t bus, uint16_t device, uint16_t function);

BAR* pci_get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);
bool has_functions(uint16_t bus, uint16_t device);

#endif 
