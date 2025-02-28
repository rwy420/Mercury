#include <driver/driver.h>
#include <common/screen.h>
#include <memory/common.h>
#include <memory/mem_manager.h>

Driver drivers[0x40];
uint8_t driver_index;

void* default_net_interface;

void init_drivers()
{
	driver_index = 0;
}

uint8_t create_driver(string name, DRIVER_TYPE type, void* init_handle, void* enable_handle, void* disbale_handle)
{
	Driver driver;

	driver.name = name;
	driver.type = type;
	driver.init_handle = init_handle;
	driver.enable_handler = enable_handle;
	driver.disbale_handler = disbale_handle;
	driver.enabled = false;
	driver.id = driver_index;

	drivers[driver_index] = driver;

	if(init_handle != NULL_PTR) drivers[driver_index].init_handle(driver_index);

	driver_index++;

	return driver.id;
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
	for(int i = 0; i < driver_index; i++)
	{
		enable_driver(i);
	}
}

void enable_driver(uint8_t id)
{
	Driver* driver = &drivers[id];

	driver->enabled = true;
	driver->enable_handler();

	printf("<Mercury> Driver '");
	printf(driver->name);
	printf("' enabled \n");

	if(driver->type == ETHERNET && driver->driver_interface)
	{
		default_net_interface = driver->driver_interface;
		printf("<Mercury> Selected '");
		printf(driver->name);
		printf("' as default ethernet driver\n");
	}
}

void disbale_driver(uint8_t id)
{
	Driver* driver = &drivers[id];
	driver->enabled = false;
	driver->disbale_handler();

	printf("<Mercury> Driver '");
	printf(driver->name);
	printf("' disabled \n");
}

void net_send(char* buffer, size_t size)
{
	if(default_net_interface)
	{
		EthernetInterface* interface = (EthernetInterface*) default_net_interface;
		interface->send(buffer, size);
	}
}
