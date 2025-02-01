#ifndef __QS__DRIVER__SATA__SATA_H
#define __QS__DRIVER__SATA__SATA_H

#include <hardware/pci.h>
#include <driver/sata/ahci.h>

void init_sata(uint32_t bar5);

#endif 
