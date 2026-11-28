#include <driver/ata/ata.h>
#include <hardware/pci.h>
#include <driver/driver.h>
#include <memory/common.h>
#include <memory/heap.h>
#include <printf.h>

Driver drivers[MAX_DRIVER_NUM];
uint8_t num_drivers;

void* default_net_interface;

uint8_t create_driver(const char* name, DRIVER_TYPE type, void* init_handle, void* enable_handle, void* disbale_handle, DeviceDescriptor* device_descriptor)
{
	Driver* driver = &drivers[num_drivers];

	driver->name = name;
	driver->type = type;
	driver->id = num_drivers;
	driver->device_descriptor = device_descriptor;
	driver->init_handle = init_handle;
	driver->enable_handler = enable_handle;
	driver->disbale_handler = disbale_handle;
	driver->enabled = 0;	

	num_drivers++;

	return driver->id;
}

void set_interface(uint8_t driver, void* interface)
{
	drivers[driver].driver_interface = interface;
}
void* get_interface(uint8_t driver)
{
	return drivers[driver].driver_interface;
}

void enable_all_drivers()
{
	for(int i = 0; i < num_drivers; i++)
	{
		enable_driver(i);
	}
}

void init_devices()
{
	for(int i = 0; i < num_drivers; i++)
	{
		Driver* driver = &drivers[i];
		if(driver->init_handle) driver->init_handle(driver);
	}

	pci_init_devices();
	ata_probe();
}

void enable_driver(uint8_t id)
{
	Driver* driver = &drivers[id];

	driver->enabled = 1;
	if(driver->enable_handler) driver->enable_handler();

	kprintf("<Mercury> Driver '%s' enabled\n", driver->name);

}

void disbale_driver(uint8_t id)
{
	Driver* driver = &drivers[id];
	driver->enabled = 0;
	if(driver->disbale_handler) driver->disbale_handler();

	kprintf("<Mercury> Driver '%s' disabled \n", driver->name);
}
