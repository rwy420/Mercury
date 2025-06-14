#include <memory/paging.h>
#include <memory/heap.h>
#include <memory/common.h>
#include <common/screen.h>
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

PageDirectory* create_kernel_pd()
{
    PageDirectory* directory = (PageDirectory*) virtual_to_physical(kmalloc_aligned(4096, 4096 ));
	if(!directory) return false;

	memset(directory, 0, sizeof(PageDirectory));

	g_kernel_pd = directory;

	PageTable* table = (PageTable*) virtual_to_physical(kmalloc_aligned(4096, 4096));
	if(!table) return false;	
	PageTable* table_3g = (PageTable*) virtual_to_physical(kmalloc_aligned(4096, 4096));
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

	return directory;
}

uint32_t virtual_to_physical(void* v_address)
{
	uint32_t v = (uint32_t) v_address;

    uint32_t pd_index = (v >> 22) & 0x3FF;
    uint32_t pt_index = (v >> 12) & 0x3FF;

    PageDirectory* pd = g_current_pd;

    uint32_t pde = pd->entries[pd_index];
    if ((pde & PDE_PRESENT) == 0)
        return 0; // Not mapped

    PageTable* pt = (PageTable*)(pde & ~0xFFF);
    pt = (PageTable*)((uint32_t)pt + 0xC0000000);

    uint32_t pte = pt->entries[pt_index];
    if ((pte & PTE_PRESENT) == 0)
        return 0;

    uint32_t phys_page_base = pte & ~0xFFF;
    uint32_t offset = v & 0xFFF;

    return phys_page_base + offset;
}

void flush_tlb_entry(uint32_t v_address)
{
	__asm__ __volatile__ ("cli; invlpg (%0); sti" : : "r" (v_address));
}

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
		SET_FRAME(entry, virtual_to_physical(table));
	}

	PageTable* table = (PageTable*) PAGE_PHYS_ADDRESS(entry);
	uint32_t* page = &table->entries[PT_INDEX((uint32_t) v_address)];

	SET_ATTRIBUTE(page, PTE_PRESENT);
	SET_ATTRIBUTE(page, PTE_RW);
	SET_FRAME(page, (uint32_t) p_address);

	//flush_tlb_entry((uint32_t) v_address);
}

void map_page(void* p_address, void* v_address)
{
	map_page_pd(g_current_pd, p_address, v_address);
}

int paging_init()
{
    PageDirectory* directory = (PageDirectory*) 0x90000;  //kmalloc_aligned(4096, 4096 * 3); //malloc(4096 * 3);
	if(!directory) return false;

	memset(directory, 0, sizeof(PageDirectory));
	//for(uint32_t i = 0; i < 1024; i++) directory->entries[i] = 0x02;

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
	__asm__ __volatile__ ("movl %CR0, %EAX; orl $0x80000011, %EAX; movl %EAX, %CR0; movl $0xC0090000, %ESP");

	register_interrupt_handler(0xE, (isr_t) handle_page_fault);

	return true;
}

void handle_page_fault(CPUState* cpu)
{
	uint32_t address = 0;
	__asm__ __volatile__("movl %%CR2, %0" : "=r" (address));

	printf("Page fault at address: 0x");
	print_hex32(address);
	printf(" error: ");
	print_hex32(cpu->error_code);
	printf("\n");

	print_hex32(cpu->eip);
	printf("\n");

	if(!(cpu->error_code & 0x1)) printf(" - Not present\n");
	if(cpu->error_code & 0x2) printf(" - Write\n");
	if(cpu->error_code & 0x4) printf(" - User\n");
	if(cpu->error_code & 0x8) printf(" - Reserved bit voilation\n");
	if(cpu->error_code & 0x10) printf(" - Instruction fetched\n");
}
