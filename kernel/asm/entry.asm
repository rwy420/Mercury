extern kernel_init
global k_entry

k_entry:
	pop eax
	pop eax
	pop ebx
	pop ecx

	mov esp, 0x90000

	push ecx
	push ebx
	push eax

	call kernel_init
