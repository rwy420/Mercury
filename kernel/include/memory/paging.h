#ifndef __QS__MEMORY__PAGING_H
#define __QS__MEMORY__PAGING_H

#include <common/types.h>

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024
#define PAGE_SIZE 4096

#define PD_INDEX(address) ((address) >> 22)
#define PT_INDEX(address) (((address) >> 12) & 0x3FF) 
#define PAGE_PHYS_ADDRESS(dir_entry) ((*dir_entry) & ~0xFFF)  
#define SET_ATTRIBUTE(entry, attr) (*(entry) |= (attr))
#define CLEAR_ATTRIBUTE(entry, attr) (*entry &= ~(attr))
#define TEST_ATTRIBUTE(entry, attr) (*entry & attr)
#define SET_FRAME(entry, address) (*entry = (*entry & ~0x7FFFF000) | address)

typedef uint32_t pt_entry_t;
typedef uint32_t pd_entry_t;

typedef enum
{
	PTE_PRESENT			= 0x01,
	PTE_RW				= 0x02,
	PTE_USER			= 0x04,
	PTE_WRITE_THROUGH	= 0x08,
	PTE_CACHE_DISABLE	= 0x10,
	PTE_ACCESSED		= 0x20,
	PTE_DIRTY			= 0x40,
	PTE_PAT				= 0x80,
	PTE_GLOBAL			= 0x100,
	PTE_FRAME			= 0xFFFFF000,
} PageTableFlags;

typedef enum
{
	PDE_PRESENT			= 0x01,
	PDE_RW				= 0x02,
	PDE_USER			= 0x04,
	PDE_WRITE_THROUGH	= 0x08,
	PDE_CAHCHE_DISABLE	= 0x10,
	PDE_ACCESSED		= 0x20,
	PDE_DIRTY			= 0x40,
	PDE_PAGE_SIZE		= 0x80,
	PDE_GLOBAL			= 0x100,
	PDE_PAT				= 0x2000,
	PDE_FRAME			= 0x7FFFF000,
} PageDirectoryFlags;

typedef struct
{
	pt_entry_t entries[PAGES_PER_TABLE];
} page_table_t;

typedef struct
{
	pd_entry_t entries[TABLES_PER_DIRECTORY];
} page_directory_t;

pt_entry_t* get_pt_entry(page_table_t* pt, uint32_t virtual_address);
pd_entry_t* get_pd_entry(page_table_t* pt, uint32_t virtual_address);
pt_entry_t* get_page(uint32_t virtual_address);

void* alloc_page(pt_entry_t* page);
void free_page(pt_entry_t* page);

void set_page_directory(page_directory_t* pd);
void flush_tlb_entry(uint32_t virtual_address);

void map_page(void* physical_address, void* virtual_address, int flags);
void unmap_page(void* virtual_address);

void paging_enable();

void handle_page_fault();

#endif 
