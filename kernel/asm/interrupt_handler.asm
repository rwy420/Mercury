extern interrupt_handler

%macro exception_interrupt_noerr 1
global handle_irq_%1
handle_irq_%1:
	mov dword [interrupt_number], %1
	push dword 0
	jmp common_interrupt_handler
%endmacro

%macro exception_interrupt 1
global handle_irq_%1
handle_irq_%1:
	mov dword [interrupt_number], %1	
	jmp common_interrupt_handler
%endmacro

%macro interrupt 1
global handle_irq_%1
handle_irq_%1:
	mov dword [interrupt_number], %1
	push dword 0
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

exception_interrupt			06	; Invalid Opcode
exception_interrupt			13	; General Protection Fault
exception_interrupt			14	; Page Fault	

interrupt 32					; PIT
interrupt 33					; Keyboard
interrupt 128					; Syscall

section .data
interrupt_number dd 0
