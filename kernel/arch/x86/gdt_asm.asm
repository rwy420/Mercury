extern test_user_function

global segments_load_gdt
global segments_load_registers
global flush_tss
global jump_usermode

segments_load_gdt:
	lgdt [esp + 4]
	ret

segments_load_registers:
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	jmp 0x08:flush_cs

flush_cs:
	ret

flush_tss:
	mov ax, (5 * 8) | 0
	ltr ax
	ret
