#include <common/types.h>
#include <driver/sata/ahci.h>
#include <common/screen.h>
#include <memory/mem_manager.h>
#include <memory/common.h>
#include <memory/paging.h>

HbaMem* g_abar;

HbaPort* probe_port(HbaMem* abar)
{
	g_abar = abar;
	HbaPort* result;
	int port_count = 0;

	for(int i = 0; i < 32; i++)
	{
		if(abar->pi & (1 << i))
		{
			HbaPort* port = &abar->ports[i];
			uint32_t type = get_type(port);

			if(type == AHCI_DEV_SATA)
			{
				printf("SATA\n");
				result = port;
				port_rebase(port, i);
			}
		}
	}

	return result;
}

void port_rebase(HbaPort* port, int port_idx)
{
	stop_cmd(port);

	void* clb = malloc_aligned(4096, 1024);
	memset(clb, 0, 1024);
	port->clb = (uint32_t) clb;
	port->clbu = (uint32_t)clb >> 32;

	void* fb = malloc_aligned(4096, 256);
	memset(fb, 0, 256);
	port->fb = (uint32_t) fb;
	port->fbu = (uint32_t) fb >> 32;

	HbaCommandHeader* cmd_header = (HbaCommandHeader*) clb;
	for(int i = 0; i < 32; i++)
	{
		cmd_header[i].prdtl = 8;
		
		void* ctba_buffer = malloc_aligned(4096, 4096);
		memset(ctba_buffer, 0, 4096);
		cmd_header[i].ctba = (uint32_t) ctba_buffer;
		cmd_header[i].ctbau = 0;
	}

	start_cmd(port);
}

uint32_t get_type(HbaPort* port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if(det != HBAPORT_DET_PRESENT) return AHCI_DEV_NULL;

	if(ipm != HBAPORT_IPM_ACTIVE) return AHCI_DEV_NULL;

	switch(port->sig)
	{
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			return AHCI_DEV_SATA;
	}
}

void start_cmd(HbaPort* port)
{
	while(port->cmd & HBA_PxCMD_CR);

	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

void stop_cmd(HbaPort* port)
{
	port->cmd &= ~HBA_PxCMD_ST;
	port->cmd &= ~HBA_PxCMD_FRE;

	while(1)
	{
		if(port->cmd & HBA_PxCMD_FR) continue;
		if(port->cmd & HBA_PxCMD_CR) continue;

		break;
	}
}

int32_t find_cmd_slot(HbaPort* port)
{
	uint32_t slots = (port->sact | port->ci);
	uint32_t cmd_slots = (g_abar->cap & 0x0F00) >> 8;

	for(int i = 0; i < cmd_slots; i++)
	{
		if((slots & 1) == 0) return i;

		slots >>= 1;
	}

	return -1;
}

bool sata_read(HbaPort* port, uint32_t startl, uint32_t starth, uint32_t count, uint8_t* buf)
{
	port->is = (uint32_t) -1;
	int spin = 0;
	int slot = find_cmd_slot(port);
	if (slot == -1) return false;
	
	HbaCommandHeader *cmd_header = (HbaCommandHeader*) port->clb;
	cmd_header += slot;
	cmd_header->cfl = sizeof(FisRegH2D)/sizeof(uint32_t);
	cmd_header->w = 0;	
	cmd_header->prdtl = (uint16_t)((count - 1) >> 4) + 1;
	
	HbaCommandTable *cmd_tbl = (HbaCommandTable*)(cmd_header->ctba);

	int i = 0;
	for (i = 0; i < cmd_header->prdtl - 1; i++)
	{
		cmd_tbl->prdt_entry[i].dba = (uint32_t) buf;
		cmd_tbl->prdt_entry[i].dbc = 8 * 1024 - 1;

		cmd_tbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;
		count -= 16;
	}

	cmd_tbl->prdt_entry[i].dba = (uint32_t) buf;
	cmd_tbl->prdt_entry[i].dbc = (count << 9) - 1;
	cmd_tbl->prdt_entry[i].i = 1;

	FisRegH2D *cmd_fis = (FisRegH2D*)(&cmd_tbl->cfis);

	cmd_fis->fis_type = FIS_TYPE_REG_H2D;
	cmd_fis->c = 1;
	cmd_fis->command = ATA_CMD_READ_DMA_EX;

	cmd_fis->lba0 = (uint8_t) startl;
	cmd_fis->lba1 = (uint8_t)(startl >> 8);
	cmd_fis->lba2 = (uint8_t)(startl >> 16);
	cmd_fis->device = 1 << 6;

	cmd_fis->lba3 = (uint8_t)(startl >> 24);
	cmd_fis->lba4 = (uint8_t) starth;
	cmd_fis->lba5 = (uint8_t)(starth >> 8);

	cmd_fis->countl = count & 0xFF;
	cmd_fis->counth = (count >> 8) & 0xFF;

	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		printf("Port is hung\n");
		return false;
	}

	port->ci = 1 << slot;

	while (1)
	{
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)
		{
			printf("Read disk error\n");
			return false;
		}
	}

	if (port->is & HBA_PxIS_TFES)
	{
		printf("Read disk error\n");
		return false;
	}

	return true;
}
