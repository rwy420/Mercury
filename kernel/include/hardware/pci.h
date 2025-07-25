#ifndef __MERCURY__HARDWARE__PCI_H
#define __MERCURY__HARDWARE__PCI_H

#define MAX_DEVICES 0xFF

#include <hardware/port.h>
#include <common/types.h>

typedef enum 
{
	INVALID,
	MM,
	IO
} BAR_TYPE;

typedef struct
{
 	BAR_TYPE type;
	uint32_t address;
	uint32_t size;
	uint8_t is_64bit, prefetchable;
} __attribute__((packed)) BAR;

typedef struct
{
	uint16_t vendor_id, device_id;
	uint16_t bus, function;
	uint8_t class_id, subclass_id, prog_if;
	uint32_t interrupt;
	BAR* bars[6];
} DeviceDescriptor;

uint32_t pci_read32(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset);
uint16_t pci_read16(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset);
uint8_t pci_read8(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset);

void pci_write32(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset, uint32_t value);
void pci_write(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset, uint16_t value);

void pci_set_command_bits(uint16_t bus, uint16_t device, uint16_t function, uint16_t mask);

void pci_enumerate_devices();
void pci_init_devices();
void get_driver(DeviceDescriptor* device_descriptor);
DeviceDescriptor* pci_get_devices();

uint16_t pci_get_vendor_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_device_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_class_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_subclass_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_interrupt(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_prog_if(uint16_t bus, uint16_t device, uint16_t function);

void pci_enable_bus_mastering(DeviceDescriptor* device);
void pci_enable_memory(DeviceDescriptor* device);

BAR* pci_get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);
int has_functions(uint16_t bus, uint16_t device);

#endif 
