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

uint32_t virtual_to_physical(void* v_address) {
    uint32_t virt = (uint32_t) v_address;

    uint32_t pd_index = virt >> 22;
    uint32_t pt_index = (virt >> 12) & 0x3FF;
    uint32_t page_offset = virt & 0xFFF;

    uint32_t pd_entry = g_current_pd->entries[pd_index];
    uint32_t* page_table = (uint32_t*)(pd_entry & ~0xFFF);
    uint32_t pt_entry = page_table[pt_index];

    uint32_t phys_frame = pt_entry & ~0xFFF;
    return phys_frame + page_offset;
}

void flush_tlb_entry(uint32_t v_address)
{
	asm volatile("invlpg (%0)" : : "r" (v_address) : "memory");
}

void map_page_pd(PageDirectory* pd, void* phys_addr, void* virt_addr)
{
    uint32_t paddr = (uint32_t)phys_addr;
    uint32_t vaddr = (uint32_t)virt_addr;

    uint32_t pd_index = PD_INDEX(vaddr);
    uint32_t pt_index = PT_INDEX(vaddr);

    // Allocate pt if necessary
    if (!(pd->entries[pd_index] & PDE_PRESENT)) {
        PageTable* pt = (PageTable*) kmalloc_aligned(4096, 4096);
        memset(pt, 0, sizeof(PageTable));
        pd->entries[pd_index] = virtual_to_physical(pt) | PDE_PRESENT | PDE_RW;
    }

    PageTable* pt = (PageTable*) PAGE_PHYS_ADDRESS(&pd->entries[pd_index]);
    pt->entries[pt_index] = paddr | PTE_PRESENT | PTE_RW;

    flush_tlb_entry(vaddr);
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
