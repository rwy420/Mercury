global segments_load_gdt
global segments_load_registers
global restore_and_switch

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

restore_and_switch:
	mov ebp, [g_current_task]
	add ebp, 5
	mov ebp, [ebp]

	mov eax, [g_current_task]
	add eax, 13
	mov eax, [eax]

	mov ecx, [g_current_task]
	add ecx, 17
	mov ecx, [ecx]

	mov edx, [g_current_task]
	add edx, 21
	mov edx, [edx]

	mov esi, [g_current_task]
	add esi, 25
	mov esi, [esi]

	mov edi, [g_current_task]
	add edi, 29
	mov edi, [edi]

	cli
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov esp, [g_current_task]
	add esp, 1
	mov esp, [esp]

	and esp, 0xFFFFFFF0

	push 0x20
	push esp
	push 0x202
	push 0x18

	mov [0x20000], eax
	mov eax, [g_current_task]
	add eax, 9
	push dword [eax]
	mov eax, [0x20000]

	iret
