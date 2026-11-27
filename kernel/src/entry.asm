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
	dd 1024		; screen width
	dd 768		; screen height
	dd 32		; screen bpp

section .multiboot.text
extern kernel_init
global _start:function (_start.end - _start)
_start:
	mov esp, 0x8FFF0
	
	lea ecx, [kernel_init - 0xC0000000] ; calculate actual location of kernel_init in physical memory

	push eax ; uint32_t magic
	push ebx ; uint32_t multiboot
	
	call ecx

	cli

.hang:	
	hlt
	jmp .hang
.end:
