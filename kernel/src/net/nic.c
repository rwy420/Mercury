#include <net/nic.h>
#include <memory/heap.h>
#include <memory/common.h>

NIC* current_nic;

NIC* create_nic()
{
	NIC* result = kmalloc(sizeof(NIC));
	memset(result, 0, sizeof(NIC));
	current_nic = result;

	return result;
}
