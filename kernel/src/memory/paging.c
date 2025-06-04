#include <memory/paging.h>

void pt_add_flags(uint32_t* pt, uint32_t flags);
void pt_delete_flags(uint32_t* pt, uint32_t flags);
void pd_add_flags(uint32_t* pt, uint32_t flags);
void pd_delete_flags(uint32_t* pt, uint32_t flags);

uint32_t* get_pd();
uint32_t* get_pt_entry(uint32_t* pt, uint32_t v_address);
uint32_t get_pd_entry(uint32_t* pd, uint32_t v_address);
uint32_t* get_page(const uint32_t v_address);
void* alloc_page(uint32_t* page);
void free_page(uint32_t* page);
void set_pd(uint32_t* pd);
void flush_tlb_entry(uint32_t v_address);
void map_page(void* p_address, void* v_address);
void unmap_page(void* v_address);
void vmap_address(uint32_t* pd, uint32_t p_address, uint32_t v_address, uint32_t flags);
void vunmap_address(uint32_t* pd, uint32_t v_address);
void create_pd(uint32_t* pd, uint32_t v_address, uint32_t flags);
void unmap_pt(uint32_t* pd, uint32_t v_address);  
void* get_p_address(uint32_t* pd, uint32_t v_address);
void paging_init();
