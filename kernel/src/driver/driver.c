#include <driver/driver.h>
#include <common/screen.h>
#include <memory/common.h>
#include <memory/mem_manager.h>

static Driver* drivers;
uint8_t driver_index;

void init_drivers()
{
	driver_index = 0;
	drivers = malloc(sizeof(Driver) * 0x40);
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

	if(init_handle != NULL_PTR) drivers[driver_index].init_handle();

	driver_index++;

	return driver.id;
}

void add_driver(Driver* driver)
{
	drivers[driver_index] = *driver;
	driver_index++;
}


void enable_driver(uint8_t id)
{
	drivers[id].enabled = true;
	drivers[id].enable_handler();

	printf("<Mercury> Driver '");
	printf(drivers[id].name);
	printf("' enabled \n");
}

void disbale_driver(uint8_t id)
{
	drivers[id].enabled = false;
	drivers[id].disbale_handler();

	printf("<Mercury> Driver '");
	printf(drivers[id].name);
	printf("' disabled \n");
}
