#include "core/types.h"
#include <driver/sata/ahci.h>
#include <core/screen.h>
#include <memory/mem_manager.h>
#include <memory/common.h>
#include <memory/paging.h>

HBA_MEM* global_abar;

HBA_PORT* probe_port(HBA_MEM* abar)
{
	global_abar = abar;
	HBA_PORT* result;
	int port_count = 0;

	for(int i = 0; i < 32; i++)
	{
		if(abar->pi & (1 << i))
		{
			HBA_PORT* port = &abar->ports[i];
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

void port_rebase(HBA_PORT* port, int port_idx)
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

	HBA_CMD_HEADER* cmd_header = (HBA_CMD_HEADER*) clb;
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

uint32_t get_type(HBA_PORT* port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if(det != HBA_PORT_DET_PRESENT) return AHCI_DEV_NULL;

	if(ipm != HBA_PORT_IPM_ACTIVE) return AHCI_DEV_NULL;

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

void start_cmd(HBA_PORT* port)
{
	while(port->cmd & HBA_PxCMD_CR);

	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

void stop_cmd(HBA_PORT* port)
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

int32_t find_cmd_slot(HBA_PORT* port)
{
	uint32_t slots = (port->sact | port->ci);
	uint32_t cmd_slots = (global_abar->cap & 0x0F00) >> 8;

	for(int i = 0; i < cmd_slots; i++)
	{
		if((slots & 1) == 0) return i;

		slots >>= 1;
	}

	return -1;
}

bool sata_read(HBA_PORT* port, uint32_t startl, uint32_t starth, uint32_t count, uint8_t* buf)
{
	port->is = (uint32_t) -1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmd_slot(port);
	if (slot == -1)
		return false;
	
	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	// PRDT entries count
														//
	
	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	//memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
 	//	(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
	
	
	int i = 0;
	// 8K bytes (16 sectors) per PRDT
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	// 8K bytes (this value should always be set to 1 less than the actual value)
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;

	// Setup command
	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	printf("\n*CMDFIS: ");
	print_hex32((uint32_t) cmdfis);

	printf("\n*CMDTBL: ");
	print_hex32((uint32_t) cmdtbl);
	
	printf("\n*CMDHDR: ");
	print_hex32((uint32_t) cmdheader);


	printf("*\n");

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;

	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode

	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);

	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;

	
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		printf("Port is hung\n");
		return false;
	}

	port->ci = 1<<slot;	// Issue command

	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			printf("Read disk error\n");
			return false;
		}
	}

	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		printf("Read disk error\n");
		return false;
	}

	return true;

}
