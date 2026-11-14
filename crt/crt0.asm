section .text

global _start
extern main

_start:
	xor ebp, ebp
	xchg bx, bx
	mov eax, [esp]
	lea ebx, [esp + 4]

	push ebx
	push eax
	call main

	jmp -2
