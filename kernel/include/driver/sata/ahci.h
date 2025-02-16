#ifndef __MERCURY__DRIVER__SATA__AHCI_H
#define __MERCURY__DRIVER__SATA__AHCI_H

#include <common/types.h>

#define SATA_SIG_ATA    0x00000101  // SATA drive
#define SATA_SIG_ATAPI  0xEB140101  // SATAPI drive
#define SATA_SIG_SEMB   0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM 0x96690101  // Port multiplier
#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SATAPI 4
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define HBA_PORT_DET_PRESENT 3
#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PxIS_TFES   (1 << 30)
#define ATA_CMD_READ_DMA_EX 0x25
#define ATA_CMD_WRITE_DMA_EX    0x35
#define HBA_PxCMD_CR    (1 << 15)
#define HBA_PxCMD_FR    (1 << 14)
#define HBA_PxCMD_FRE   (1 << 4)
#define HBA_PxCMD_SUD   (1 << 1)
#define HBA_PxCMD_ST    (1 << 0)
#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08

typedef enum
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;

typedef struct
{
	uint32_t   clb;        // 0x00, command list base address, 1K-byte aligned
    uint32_t   clbu;       // 0x04, command list base address upper 32 bits
    uint32_t   fb;     // 0x08, FIS base address, 256-byte aligned
    uint32_t   fbu;        // 0x0C, FIS base address upper 32 bits
    uint32_t   is;     // 0x10, interrupt status
    uint32_t   ie;     // 0x14, interrupt enable
    uint32_t   cmd;        // 0x18, command and status
    uint32_t   rsv0;       // 0x1C, Reserved
    uint32_t   tfd;        // 0x20, task file data
    uint32_t   sig;        // 0x24, signature
    uint32_t   ssts;       // 0x28, SATA status (SCR0:SStatus)
    uint32_t   sctl;       // 0x2C, SATA control (SCR2:SControl)
    uint32_t   serr;       // 0x30, SATA error (SCR1:SError)
    uint32_t   sact;       // 0x34, SATA active (SCR3:SActive)
    uint32_t   ci;     // 0x38, command issue
    uint32_t   sntf;       // 0x3C, SATA notification (SCR4:SNotification)
    uint32_t   fbs;        // 0x40, FIS-based switch control
    uint32_t   rsv1[11];   // 0x44 ~ 0x6F, Reserved
    uint32_t   vendor[4];  // 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef struct
{
	uint32_t cap;
	uint32_t ghc;
	uint32_t is;
	uint32_t pi;
	uint32_t vs;
	uint32_t ccc_ctl;
	uint32_t ccc_pts;
	uint32_t em_loc;
	uint32_t em_ctl;
	uint32_t cap2;
	uint32_t bohc;
	
	uint8_t rsv[0xA0-0x2C];
	uint8_t vendor[0x100 - 0xA0];

	HBA_PORT ports[1];
} __attribute__((packed)) HBA_MEM;

typedef struct tagHBA_CMD_HEADER
{
    // DW0
    uint8_t    cfl:5;      // Command FIS length in DWORDS, 2 ~ 16
    uint8_t    a:1;        // ATAPI
    uint8_t    w:1;        // Write, 1: H2D, 0: D2H
    uint8_t    p:1;        // Prefetchable

    uint8_t    r:1;        // Reset
    uint8_t    b:1;        // BIST
    uint8_t    c:1;        // Clear busy upon R_OK
    uint8_t    rsv0:1;     // Reserved
    uint8_t    pmp:4;      // Port multiplier port

    uint16_t    prdtl;      // Physical region descriptor table length in entries

    // DW1
    volatile
    uint32_t   prdbc;      // Physical region descriptor byte count transferred

    // DW2, 3
    uint32_t   ctba;       // Command table descriptor base address
    uint32_t   ctbau;      // Command table descriptor base address upper 32 bits

    // DW4 - 7
    uint32_t   rsv1[4];    // Reserved
} __attribute__((packed)) HBA_CMD_HEADER;

typedef struct
{
	// DWORD 0
	uint8_t  fis_type;	// FIS_TYPE_REG_H2D

	uint8_t  pmport:4;	// Port multiplier
	uint8_t  rsv0:3;		// Reserved
	uint8_t  c:1;		// 1: Command, 0: Control

	uint8_t  command;	// Command register
	uint8_t  featurel;	// Feature register, 7:0
	
	// DWORD 1
	uint8_t  lba0;		// LBA low register, 7:0
	uint8_t  lba1;		// LBA mid register, 15:8
	uint8_t  lba2;		// LBA high register, 23:16
	uint8_t  device;		// Device register

	// DWORD 2
	uint8_t  lba3;		// LBA register, 31:24
	uint8_t  lba4;		// LBA register, 39:32
	uint8_t  lba5;		// LBA register, 47:40
	uint8_t  featureh;	// Feature register, 15:8

	// DWORD 3
	uint8_t  countl;		// Count register, 7:0
	uint8_t  counth;		// Count register, 15:8
	uint8_t  icc;		// Isochronous command completion
	uint8_t  control;	// Control register

	// DWORD 4
	uint8_t  rsv1[4];	// Reserved
} __attribute__((packed)) FIS_REG_H2D;

typedef struct
{
	uint32_t dba;		// Data base address
	uint32_t dbau;		// Data base address upper 32 bits
	uint32_t rsv0;		// Reserved

	// DW3
	uint32_t dbc:22;		// Byte count, 4M max
	uint32_t rsv1:9;		// Reserved
	uint32_t i:1;		// Interrupt on completion
} __attribute__((packed)) HBA_PRDT_ENTRY;

typedef struct
{
	// 0x00
	uint8_t  cfis[64];	// Command FIS

	// 0x40
	uint8_t  acmd[16];	// ATAPI command, 12 or 16 bytes

	// 0x50
	uint8_t  rsv[48];	// Reserved

	// 0x80
	HBA_PRDT_ENTRY	prdt_entry[1];	// Physical region descriptor table entries, 0 ~ 65535
} __attribute__((packed)) HBA_CMD_TBL;

typedef struct
{
	void* clb;
	void* fb;
	void* ctba[32];
	HBA_PORT* port;
} PortData;

HBA_PORT* probe_port(HBA_MEM* abar);
void port_rebase(HBA_PORT* port, int port_idx);
uint32_t get_type(HBA_PORT* port);

void start_cmd(HBA_PORT* port);
void stop_cmd(HBA_PORT* port);
int32_t find_cmd_slot(HBA_PORT* port);

bool sata_read(HBA_PORT* port, uint32_t startl, uint32_t starth, uint32_t count, uint8_t* buffer);

#endif 
