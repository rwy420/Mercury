global segments_load_gdt
global segments_load_registers
global save_task

extern g_current_task

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

save_task:
	pusha
	push ds
	push es
	push fs
	push gs

	mov eax, [g_current_task]
	mov [eax + 1], esp
	mov [eax + 5], ebp
	mov eax, [esp + 36]
	mov [g_current_task + 9], eax

	pop gs
	pop fs
	pop es
	pop ds
	popa

	ret
