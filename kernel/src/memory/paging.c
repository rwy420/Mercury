#include <core/types.h>
#include <memory/paging.h>
#include <memory/mem_manager.h>
#include <memory/common.h>
#include <core/screen.h>
#include <hardware/interrupts.h>

extern uint32_t kernel_start_address;

page_directory_t* current_page_directory = 0;

pt_entry_t* get_pt_entry(page_table_t* pt, uint32_t virtual_address)
{
	if(pt) return &pt->entries[PT_INDEX(virtual_address)];

	return 0;
}

pd_entry_t* get_pd_entry(page_table_t* pt, uint32_t virtual_address)
{
	if(pt) return &pt->entries[PT_INDEX(virtual_address)];

	return 0;
}

pt_entry_t* get_page(uint32_t virtual_address)
{
	page_directory_t* pd = current_page_directory;

	pd_entry_t* entry = &pd->entries[PD_INDEX(virtual_address)];
	page_table_t* table = (page_table_t*) PAGE_PHYS_ADDRESS(entry);

	pt_entry_t* page = &table->entries[PT_INDEX(virtual_address)];

	return page;
}

void* alloc_page(pt_entry_t* page)
{
	void* block = alloc_blocks(1);
	
	if(block)
	{
		SET_FRAME(page, (uint32_t) block);
		SET_ATTRIBUTE(page, PTE_PRESENT);
	}

	return block;
}

void free_page(pt_entry_t* page)
{
	void* address = (void*) PAGE_PHYS_ADDRESS(page);

	if(address) free_blocks(address, 1);
	
	CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}

void set_page_directory(page_directory_t* pd)
{
	current_page_directory = pd;

	__asm__ __volatile__ ("movl %%EAX, %%CR3" : : "a" (current_page_directory));
}

void flush_tlb_entry(uint32_t virtual_address)
{
	__asm__ __volatile__ ("cli; invlpg (%0); sti" : : "r" (virtual_address));
}

void map_page(void* physical_address, void* virtual_address)
{
	page_directory_t* pd = current_page_directory;

	pd_entry_t* entry = &pd->entries[PD_INDEX((uint32_t) virtual_address)];

	if((*entry & PTE_PRESENT) != PTE_PRESENT)
	{
		page_table_t* table = (page_table_t*) /*alloc_blocks(1);*/ malloc(4096);

		memset(table, 0, sizeof(page_table_t));

		pd_entry_t* entry = &pd->entries[PD_INDEX((uint32_t) virtual_address)];
		SET_ATTRIBUTE(entry, PDE_PRESENT);
		//SET_ATTRIBUTE(entry, PDE_RW);
		SET_FRAME(entry, (uint32_t) table);
	}

	page_table_t* table = (page_table_t*) PAGE_PHYS_ADDRESS(entry);

	pt_entry_t* page = &table->entries[PT_INDEX((uint32_t) virtual_address)];

	SET_ATTRIBUTE(page, PTE_PRESENT);
	SET_FRAME(page, (uint32_t) physical_address);

	flush_tlb_entry((uint32_t) virtual_address);
}

void unmap_page(void* virtual_address)
{
	pt_entry_t* page = get_page((uint32_t) virtual_address);
	SET_FRAME(page, 0);
	CLEAR_ATTRIBUTE(page, PTE_PRESENT);
}

void paging_enable()
{
	page_directory_t* directory = (page_directory_t*) alloc_blocks(3); //malloc(4096 * 3);
	memset(directory, 0, sizeof(page_directory_t));
	for(uint32_t i = 0; i < 1024; i++) directory->entries[i] = 0x02;

	page_table_t* table = (page_table_t*) alloc_blocks(1); //malloc(4096);
	memset(table, 0x0, sizeof(page_table_t));

	for(uint32_t i = 0, frame = 0, virt = 0; i < 1024; i++, frame += PAGE_SIZE, virt += PAGE_SIZE)
	{
		pt_entry_t page = 0;
		SET_ATTRIBUTE(&page, PTE_PRESENT);
		SET_ATTRIBUTE(&page, PTE_RW);
		SET_FRAME(&page, frame);

		table->entries[PT_INDEX(virt)] = page;
	}

	pd_entry_t* entry = &directory->entries[PD_INDEX(0x00000000)];
	SET_ATTRIBUTE(entry, PDE_PRESENT);
	SET_ATTRIBUTE(entry, PDE_RW);
	SET_FRAME(entry, (uint32_t) table);
	
	printf("<Mercury> Virtual memory critical step 1 success\n");

	set_page_directory(directory);

	printf("<Mercury> Virtual memory critical step 2 success\n");

	__asm__ __volatile__ ("orl $0x80000011, %EAX; movl %EAX, %CR0");

	printf("<Mercury> Virtual memory critical step 3 success\n");

	register_interrupt_handler(14, handle_page_fault);
}

void handle_page_fault()
{
	uint32_t bad_address = 0;
	__asm__ __volatile__ ("movl %%CR2, %0" : "=r" (bad_address));

	printf("Page fault at address: 0x");
	print_hex32(bad_address);
	printf("\n");
}
