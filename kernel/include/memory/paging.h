#ifndef __MERCURY__MEMORY__PAGING_H
#define __MERCURY__MEMORY__PAGING_H

#include <common/types.h>

#define PAGES_PER_TABLE 1024
#define TABLES_PER_DIRECTORY 1024
#define PAGE_SIZE 4096

#define PD_INDEX(address) ((address) >> 22)
#define PT_INDEX(address) (((address) >> 12) & 0x3FF)
#define PAGE_PHYS_ADDRESS(dir_entry) ((*dir_entry) & ~0xFFF)
#define SET_FRAME(entry, address) (*entry = (*entry & ~0x7FFFF000) | address)

#define SET_ATTRIBUTE(entry, attr) (*entry |= attr)
#define CLEAR_ATTRIBUTE(entry, addr) (*entry &= ~attr)
#define TEST_ATTRIBUTE(entry, attr) (*entry & attr);

typedef enum
{
	PTE_PRESENT = 0x01,
	PTE_RW = 0x02,
	PTE_USER = 0x04,
	PTE_WRITE_THROUGH = 0x08,
	PTE_CACHE_DISABLE = 0x10,
	PTE_ACCESSED = 0x20,
	PTE_DIRTY = 0x40,
	PTE_PAT = 0x80,
	PTE_GLOBAL = 0x100,
	PTE_FRAME = 0x7FFFF000
} PAGE_TABLE_FLAGS;

typedef enum
{
	PDE_PRESENT = 0x01,
	PDE_RW = 0x02,
	PDE_USER = 0x04,
	PDE_WRITE_THROUGH = 0x08,
	PDE_CACHE_DISABLE = 0x10,
	PDE_ACCESSED = 0x20,
	PDE_DIRTY = 0x40,
	PDE_PAGE_SIZE = 0x80,
	PDE_GLOBAL = 0x100,
	PDE_PAT = 0x2000,
	PDE_FRAME = 0x7FFFF000
} PAGE_DIRECTORY_FLAGS;

typedef struct
{
	uint32_t entries[PAGES_PER_TABLE];
} PageTable;

typedef  struct
{
	uint32_t entries[TABLES_PER_DIRECTORY];
} PageDirectory;

void pt_add_flags(uint32_t* pt, uint32_t flags);
void pt_delete_flags(uint32_t* pt, uint32_t flags);
void pd_add_flags(uint32_t* pd, uint32_t flags);
void pd_delete_flags(uint32_t* pd, uint32_t flags);

void set_pd(PageDirectory* pd);
PageDirectory* get_pd();
uint32_t* get_pt_entry(uint32_t* pt, uint32_t v_address);
uint32_t get_pd_entry(uint32_t* pd, uint32_t v_address);
uint32_t* get_page(const uint32_t v_address);
void* alloc_page(uint32_t* page);
void free_page(uint32_t* page);
void flush_tlb_entry(uint32_t v_address);
void map_page_pd(PageDirectory* pd, void* p_address, void* v_address);
void map_page(void* p_address, void* v_address);
void unmap_page(void* v_address);
void vmap_address(uint32_t* pd, uint32_t p_address, uint32_t v_address, uint32_t flags);
void vunmap_address(uint32_t* pd, uint32_t v_address);
void create_pd(uint32_t* pd, uint32_t v_address, uint32_t flags);
void unmap_pt(uint32_t* pd, uint32_t v_address);  
void* get_p_address(uint32_t* pd, uint32_t v_address);
int paging_init();
void handle_page_fault();

#endif 
