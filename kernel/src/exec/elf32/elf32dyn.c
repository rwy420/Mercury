#include <exec/elf32/elf32sym.h>
#include <vm.h>
#include <exec/elf32/elf32.h>
#include <exec/elf32/elf32dynlib.h>
#include <memory/paging.h>
#include <exec/elf32/elf32dyn.h>
#include <memory/common.h>

extern PageDirectory* g_kernel_pd;

int elf32_handle_ptdynamic(Elf32_Phdr* phdr, Elf32_Load_Info* load_info)
{
	set_pd(load_info->pd);
	Elf32_Dyn* dyn = (Elf32_Dyn*) (load_info->offset + phdr->p_vaddr);
	uint32_t count = phdr->p_memsz / sizeof(Elf32_Dyn);
	char* needed_paths[16];

	for(int i = 0; i < count; i++)
	{
		switch(dyn[i].d_tag)
		{
			case DT_NULL:
				// Doesn't seem to always end at the entry at count, therefore we'll exit after a DT_NULL entry
				goto pt_dynamic_end;
				break;
			case DT_NEEDED:
				needed_paths[load_info->needed_count++] = (char*) dyn[i].d_un.d_ptr;
				break;
			case DT_PLT_REL_SIZE:
				load_info->jmprel_size = dyn[i].d_un.d_val;
				break;
			case DT_HASH:
				load_info->hash_off = dyn[i].d_un.d_ptr;
				break;
			case DT_STRTAB:
				load_info->dynstr_off = dyn[i].d_un.d_ptr;
				break;
			case DT_SYMTAB:
				load_info->dynsym_off = dyn[i].d_un.d_ptr;
				break;
			case DT_REL:
				load_info->relocations = (Elf32_Rel*) (load_info->offset + dyn[i].d_un.d_ptr);
				break;
			case DT_RELSZ:
				load_info->rel_size = dyn[i].d_un.d_val;
				break;
			case DT_RELENT:
				uint32_t rel_entry_size = dyn[i].d_un.d_val;
				if(rel_entry_size != sizeof(Elf32_Rel)) goto error;
				break;
			case DT_PLT_REL:
				int plt_rel_type = dyn[i].d_un.d_val;
				if(plt_rel_type == DT_RELA) dbgprintf("UNSUPPORTED PLT_REL_TYPE = DT_RELA\n");
				break;
			case DT_JMPREL:
				load_info->jmp_relocations = (Elf32_Rel*) (load_info->offset +  dyn[i].d_un.d_ptr);
				break;
		}
	}

pt_dynamic_end:
	for(int i = 0; i < load_info->needed_count; i++) load_info->needed[i] = elf32_handle_dtneeded(needed_paths[i], load_info);

	set_pd(g_kernel_pd);
	return 0;

error:
	set_pd(g_kernel_pd);
	return 1;
}

int elf32_relocate(Elf32_Rel* rel, Elf32_Load_Info* load_info)
{
	if(load_info->relocations)
	{
		uint32_t relocation_count = load_info->rel_size / sizeof(Elf32_Rel);

		for(uint32_t i = 0; i < relocation_count; i++)
		{
			elf32_relocate_sym(&load_info->relocations[i], load_info);
		}
	}

	if(load_info->jmp_relocations)
	{
		uint32_t relocation_count = load_info->jmprel_size / sizeof(Elf32_Rel);

		for(uint32_t i = 0; i < relocation_count; i++)
		{
			elf32_relocate_sym(&load_info->jmp_relocations[i], load_info);
		}
	}

	return 0;
}

int elf32_relocate_sym(Elf32_Rel* rel, Elf32_Load_Info* load_info)
{
	uint32_t type = ELF32_R_TYPE(rel->r_info);
	uint32_t sym = ELF32_R_SYM(rel->r_info);
	uint32_t* target = (uint32_t*) (load_info->offset + rel->r_offset);

	switch(type)
	{
		case R_386_RELATIVE:
			if(sym != 0) return -1;
				
			*target += load_info->offset;

			break;
	
		case R_386_JMP_SLOT:
		case R_386_GLOB_DAT:
			char* symlit = elf32_dynsym_get_lit(sym, load_info);
			void* addr = elf32_total_lookup(symlit, load_info);

			*target = (uint32_t) addr;

			break;

		default:
			dbgprintf("Unhandled relocation %u\n", type);
			break;
	}

	return 0;
}
