global restore_and_switch
extern g_current_task

restore_and_switch:
	mov ebp, [g_current_task]
	add ebp, 5
	mov ebp, [ebp]

	mov eax, [g_current_task]
	add eax, 13
	mov eax, [eax]

	mov ebx, [g_current_task]
	add ebx, 17
	mov ebx, [ebx]

	mov ecx, [g_current_task]
	add ecx, 21
	mov ecx, [ecx]

	mov edx, [g_current_task]
	add edx, 25
	mov edx, [edx]

	mov esi, [g_current_task]
	add esi, 29
	mov esi, [esi]

	mov edi, [g_current_task]
	add edi, 33
	mov edi, [edi]

	cli
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov esp, [g_current_task]
	add esp, 1
	mov esp, [esp]

	and esp, 0xFFFFFFF0

	push 0x10
	push esp
	push 0x202
	push 0x8

	mov [0x20000], eax
	mov eax, [g_current_task]
	add eax, 9
	push dword [eax]
	mov eax, [0x20000]

	iret
