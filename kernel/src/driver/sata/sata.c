#include <driver/sata/sata.h>
#include <memory/mem_manager.h>
#include <common/screen.h>

HBA_MEM* abar;

void init_sata(uint32_t bar5)
{
	abar = (HBA_MEM*) bar5;
	HBA_PORT* port = probe_port(abar);

	/*uint8_t* buffer = malloc_aligned(4096, 8192);
	bool success = sata_read(port, 0, 0, 1, buffer);

	if(!success) printf("DISK FAILURE!!\n");

	for(int i = 0; i < 512; i++)
	{
		print_hex(buffer[i]);
	}*/
}
