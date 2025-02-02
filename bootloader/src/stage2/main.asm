;org 0x7E00
org 0x8E00
bits 16

[map all stage2.map]

entry:
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov sp, 0x8E00

	mov si, stage2_message
	call puts

	cli
	
	call enable_a20
	call load_gdt

	mov eax, cr0
	or al, 1
	mov cr0, eax


	jmp dword 8h:.pmode

.pmode:
	[bits 32]

	mov ax, 0x10
	mov ds, ax
	mov ss, ax

	; Kernel is located at 0xA000
	; We want it at 0x100000

	call 8h:relocate_kernel

.enter_kernel:
	;jmp 8h:100000h

	cli 
	hlt

relocate_kernel:
	[bits 32]
	push eax
	push ebx

	;mov eax, 0x100000
	;mov ebx, 0xA000
	xor ebx, ebx

.reloc_loop:
	[bits 32]

	mov eax, [0xA000 + ebx]
	mov [0x100000 + ebx], eax
	add ebx, 4

	cmp ebx, 40000
	jne .reloc_loop

.done:
	[bits 32]
	pop ebx
	pop eax

	ret


; --> si: Pointer to the string to print
puts:
	[bits 16]
	push si
	push ax
	push bx

.loop:
	[bits 16]
	lodsb
	or al, al
	jz .done

	mov ah, 0x0E
	int 0x10

	jmp .loop

.done:
	[bits 16]
	pop bx
	pop ax
	pop si
	ret

enable_a20:
	[bits 16]
	call a20_wait_input
	mov al, kbd_controller_disable
	out kbd_controller_command_port, al

	call a20_wait_input
	mov al, kbd_controller_read_control_output
	out kbd_controller_command_port, al

	call a20_wait_output
	in al, kbd_controller_data_port
	push eax

	call a20_wait_input
	mov al, kbd_controller_write_control_output
	out kbd_controller_command_port, al

	call a20_wait_input
	pop eax
	or al, 2
	out kbd_controller_data_port, al

	call a20_wait_input
	mov al, kbd_controller_enable
	out kbd_controller_command_port, al

	call a20_wait_input
	ret


a20_wait_input:
	[bits 16]
	in al, kbd_controller_command_port
	test al, 2
	jnz a20_wait_input
	ret

a20_wait_output:
	[bits 16]
	in al, kbd_controller_command_port
	test al, 1
	jz a20_wait_output
	ret

load_gdt:
	[bits 16]
	lgdt [g_gdt_descriptor]
	ret 

kbd_controller_data_port:				equ 0x60
kbd_controller_command_port:			equ 0x64
kbd_controller_disable:					equ 0xAD
kbd_controller_enable:					equ 0xAE
kbd_controller_read_control_output:		equ 0xD0
kbd_controller_write_control_output:	equ 0xD1

stage2_message:							db "Boot stage 2", 0x0A, 0x0D, 0
relocate_done_message:					db "Kernel relocated", 0x0A, 0x0D, 0

g_gdt_start:
	.null_descriptor:
            dq 0

	.code_descriptor:
            ; 32-bit code segment
            dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
            dw 0                        ; base (bits 0-15) = 0x0
            db 0                        ; base (bits 16-23)
            db 10011010b                ; access (present, ring 0, code segment, executable, direction 0, readable)
            db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
            db 0                        ; base high

	.data_descriptor:
            ; 32-bit data segment
            dw 0FFFFh                   ; limit (bits 0-15) = 0xFFFFF for full 32-bit range
            dw 0                        ; base (bits 0-15) = 0x0
            db 0                        ; base (bits 16-23)
            db 10010010b                ; access (present, ring 0, data segment, executable, direction 0, writable)
            db 11001111b                ; granularity (4k pages, 32-bit pmode) + limit (bits 16-19)
            db 0                        ; base high
g_gdt_end:

g_gdt_descriptor:
	dw g_gdt_end - g_gdt_start - 1
	dd g_gdt_start

CODE_SEG equ g_gdt_start.code_descriptor - g_gdt_start
DATA_SEG equ g_gdt_start.data_descriptor - g_gdt_start

gdt_reload_segments:
	jmp CODE_SEG:.reload_cs

.reload_cs:
	mov ds, [DATA_SEG]
	mov cx, [CODE_SEG]
	ret
