#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/common.h>
#include <common/screen.h>
#include <hardware/interrupts.h>
#include <vesa.h>

extern void v_kernel_start();

PageDirectory* g_current_pd = 0;
PageDirectory* g_kernel_pd = 0;

void pt_add_flags(uint32_t* pt, uint32_t flags)
{
	*pt |= flags;
}

void pt_delete_flags(uint32_t* pt, uint32_t flags)
{
	*pt &= ~flags;
}

void pd_add_flags(uint32_t* pd, uint32_t flags)
{
	*pd |= flags;
}

void pd_delete_flags(uint32_t* pd, uint32_t flags)
{
	*pd &= ~flags;
}

void set_pd(PageDirectory* pd)
{
	g_current_pd = pd;
	__asm__ __volatile__("movl %%EAX, %%CR3" : : "a" (g_current_pd));
}

PageDirectory* get_pd()
{
	return g_current_pd;
}

uint32_t* get_pt_entry(uint32_t* pt, uint32_t v_address);
uint32_t get_pd_entry(uint32_t* pd, uint32_t v_address);
uint32_t* get_page(const uint32_t v_address);
void* alloc_page(uint32_t* page);
void free_page(uint32_t* page);

void flush_tlb_entry(uint32_t v_address);

void map_page_pd(PageDirectory* pd, void* p_address, void* v_address)
{
	uint32_t* entry = &pd->entries[PD_INDEX((uint32_t) v_address)];

	if((*entry & PDE_PRESENT) != PDE_PRESENT)
	{
		PageTable* table = (PageTable*) kmalloc_aligned(4096, 4096);
		if(!table)
		{
			printf("ERROR ALLOCATING\n");
			return;
		}

		memset(table, 0, sizeof(PageTable));

		SET_ATTRIBUTE(entry, PDE_PRESENT);
		SET_ATTRIBUTE(entry, PDE_RW);
		SET_FRAME(entry, (uint32_t) table);
	}

	PageTable* table = (PageTable*) PAGE_PHYS_ADDRESS(entry);
	uint32_t* page = &table->entries[PT_INDEX((uint32_t) v_address)];

	SET_ATTRIBUTE(page, PTE_PRESENT);
	SET_FRAME(page, (uint32_t) p_address);
}




void map_page(void* p_address, void* v_address)
{
	map_page_pd(g_current_pd, p_address, v_address);
}

void unmap_page(void* v_address);
void vmap_address(uint32_t* pd, uint32_t p_address, uint32_t v_address, uint32_t flags);
void vunmap_address(uint32_t* pd, uint32_t v_address);
void create_pd(uint32_t* pd, uint32_t v_address, uint32_t flags);
void unmap_pt(uint32_t* pd, uint32_t v_address);  
void* get_p_address(uint32_t* pd, uint32_t v_address);

int paging_init()
{
    PageDirectory* directory = (PageDirectory*) 0x90000;  //kmalloc_aligned(4096, 4096 * 3); //malloc(4096 * 3);
	if(!directory) return false;

	memset(directory, 0, sizeof(PageDirectory));
	for(uint32_t i = 0; i < 1024; i++) directory->entries[i] = 0x02;

	g_kernel_pd = directory;

	PageTable* table = (PageTable*) 0x93000; //kmalloc_aligned(4096, 4096); //malloc(4096);
	if(!table) return false;	
	PageTable* table_3g = (PageTable*) 0x94000; //kmalloc_aligned(4096, 4096); //malloc(4096);
	if(!table_3g) return false;	

	memset(table, 0x0, sizeof(PageTable));
	memset(table_3g, 0x0, sizeof(PageTable));

	for(uint32_t i = 0, frame = 0, virt = 0; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
	{
		uint32_t page = 0;
		SET_ATTRIBUTE(&page, PTE_PRESENT);
		SET_ATTRIBUTE(&page, PTE_RW);
		SET_FRAME(&page, frame);

		table_3g->entries[PT_INDEX(virt)] = page;
	}

	for(uint32_t i = 0x0, frame = 0x0, virt = 0xC0000000; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
	{
		uint32_t page = 0;
		SET_ATTRIBUTE(&page, PTE_PRESENT);
		SET_ATTRIBUTE(&page, PTE_RW);
		SET_FRAME(&page, frame);

		table->entries[PT_INDEX(virt)] = page;
	}

	uint32_t* entry = &directory->entries[PD_INDEX(0xC0000000)];
	SET_ATTRIBUTE(entry, PDE_PRESENT);
	SET_ATTRIBUTE(entry, PDE_RW);
	SET_FRAME(entry, (uint32_t) table);

	uint32_t* entry2 = &directory->entries[PD_INDEX(0x00000000)];
	SET_ATTRIBUTE(entry2, PDE_PRESENT);
	SET_ATTRIBUTE(entry2, PDE_RW);
	SET_FRAME(entry2, (uint32_t) table_3g);

	set_pd(directory);
	__asm__ __volatile__ ("movl %CR0, %EAX; orl $0x80000011, %EAX; movl %EAX, %CR0");

	register_interrupt_handler(0xE, (isr_t) handle_page_fault);

	v_kernel_start();
	
	return false;
}

void handle_page_fault()
{
	uint32_t address = 0;
	__asm__ __volatile__("movl %%CR2, %0" : "=r" (address));

	printf("Page fault at address: 0x");
	print_hex32(address);
	printf("\n");
}
