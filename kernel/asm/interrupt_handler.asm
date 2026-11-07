extern interrupt_handler

%macro interrupt 1
global handle_irq_%1
handle_irq_%1:
	mov dword [interrupt_number], %1
	push dword 0
	jmp common_interrupt_handler
%endmacro

%macro error_interrupt 1
global handle_irq_%1
handle_irq_%1:
	mov dword [interrupt_number], %1	
	;push dword %1 need to remove this when the cpu pushes the error code already
	; TODO: make a separate exception interrupt handler macro for exception where the CPU does not push an error code
	jmp common_interrupt_handler
%endmacro

common_interrupt_handler:
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push esp
	push dword [interrupt_number]

	;cld
	call interrupt_handler
	mov esp, eax

	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp

	add esp, 4

	iret

interrupt 00
interrupt 01
interrupt 02
interrupt 03
interrupt 04
interrupt 05
interrupt 06
interrupt 07
interrupt 08
interrupt 09
interrupt 10
interrupt 32
interrupt 33
interrupt 41
interrupt 43
interrupt 49
interrupt 128

error_interrupt 11
error_interrupt 12
error_interrupt 13
error_interrupt 14	
error_interrupt 15

section .data
interrupt_number dd 0
