#include <driver/driver.h>
#include <common/screen.h>

static struct Driver* drivers[256];
uint8_t driver_index = 0;

uint8_t create_driver(uint32_t interrupt, string name, enum DriverType type, isr_t interrupt_handler, 
		enable_handler_t enable_handler, disbale_handler_t disbale_handler)
{

	struct Driver driver;

	driver.interrupt = interrupt;
	driver.name = name;
	driver.type = type;
	driver.interrupt_handler = interrupt_handler;
	driver.enable_handler = enable_handler;
	driver.disbale_handler = disbale_handler;
	driver.enabled = false;
	driver.id = driver_index;

	drivers[driver_index] = &driver;
	driver_index++;

	register_interrupt_handler(interrupt, interrupt_handler);

	return driver.id;
}

void add_driver(struct Driver* driver)
{
	drivers[driver_index] = driver;
	driver_index++;
}


void enable_driver(uint8_t id)
{
	drivers[id]->enabled = true;
	drivers[id]->enable_handler();

	printf("<Mercury> Driver '");
	printf(drivers[id]->name);
	printf("' enabled \n");
}

void disbale_driver(uint8_t id)
{
	drivers[id]->enabled = false;
	drivers[id]->disbale_handler();

	printf("<Mercury> Driver '");
	printf(drivers[id]->name);
	printf("' disabled \n");
}
