section .text

global _start
extern main

_start:
	xor ebp, ebp
	xchg bx, bx
	mov eax, [esp]

	push eax
	call main

	jmp -2
