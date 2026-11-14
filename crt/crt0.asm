section .text

global _start
extern main

_start:
	xor ebp, ebp
	mov eax, [esp]
	lea ebx, [esp + 4]

	push ebx
	push eax
	call main

	; Exit syscall
	mov ebx, eax
	mov eax, 1
	int 0x80

	; Shouldn't get here
	cd2
