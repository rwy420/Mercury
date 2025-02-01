#ifndef __QS__HARDWARE__PCI_H
#define __QS__HARDWARE__PCI_H

#include <hardware/port.h>
#include <core/types.h>
#include <driver/driver.h>

enum BARType
{
	MM = 0,
	IO = 1
};

typedef struct
{
	bool prefetchable;
	uint32_t address;
	uint32_t size;
 	enum BARType type;
} __attribute__((packed)) BAR;

uint32_t pci_read(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset);
void pci_write(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset, uint32_t value);

void pci_enumerate_devices(bool debug);
struct Driver* get_driver(uint16_t vendor, uint16_t device);

uint16_t pci_get_vendor_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_device_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_class_id(uint16_t bus, uint16_t device, uint16_t function);
uint16_t pci_get_subclass_id(uint16_t bus, uint16_t device, uint16_t function);

BAR* pci_get_bar(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);
bool has_functions(uint16_t bus, uint16_t device);

#endif 
