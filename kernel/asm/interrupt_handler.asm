section .text

extern interrupt_handler

%macro no_error_interrupt_handler 1
global handle_irq_%1
handle_irq_%1:
	push dword %1
	jmp common_interrupt_handler
%endmacro

%macro error_interrupt_handler 1
global handle_irq_%1
handle_irq_%1:
	push dword %1
	jmp common_interrupt_handler
%endmacro

common_interrupt_handler:
	push dword 0

	push esp 
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax

	call interrupt_handler

	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop esp

	add esp, 8

	iret

no_error_interrupt_handler 00
no_error_interrupt_handler 01
no_error_interrupt_handler 02
no_error_interrupt_handler 03
no_error_interrupt_handler 04
no_error_interrupt_handler 05
no_error_interrupt_handler 06
no_error_interrupt_handler 07
no_error_interrupt_handler 08
no_error_interrupt_handler 09
no_error_interrupt_handler 10
no_error_interrupt_handler 11
no_error_interrupt_handler 12
no_error_interrupt_handler 13
no_error_interrupt_handler 14	
no_error_interrupt_handler 15
no_error_interrupt_handler 32
no_error_interrupt_handler 33
no_error_interrupt_handler 49
no_error_interrupt_handler 128
