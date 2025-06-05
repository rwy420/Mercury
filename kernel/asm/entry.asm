extern kernel_main
global k_entry

k_entry:
	pop eax
	pop eax
	pop ebx
	pop ecx

	mov esp, 0x100000

	push ecx
	push ebx
	push eax
	call kernel_main
