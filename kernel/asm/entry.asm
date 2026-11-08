MBALIGN		equ 1 << 0
MEMINFO		equ 1 << 1
VIDEO_MODE	equ 1 << 2
MBFLAGS		equ MBALIGN | MEMINFO | VIDEO_MODE
MAGIC		equ 0x1BADB002
CHECKSUM	equ -(MAGIC + MBFLAGS)

section .multiboot.data
align 4
	dd MAGIC
	dd MBFLAGS
	dd CHECKSUM
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd 1024
	dd 768
	dd 32

section .multiboot.text
extern kernel_init
global _start:function (_start.end - _start)
_start:
	mov esp, 0x8FFF0
	mov ecx, kernel_init
	sub ecx, 0xC0000000; 

	push eax
	push ebx
	
	call ecx

	cli

.hang:	
	hlt
	jmp .hang
.end:
