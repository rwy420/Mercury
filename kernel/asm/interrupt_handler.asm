extern interrupt_handler

%macro interrupt 1
global handle_irq_%1
handle_irq_%1:
	push dword %1
	jmp common_interrupt_handler
%endmacro

common_interrupt_handler:
	push dword 0

	mov [0x20000], eax
	mov eax, [esp + 8]
	push eax	
	mov eax, [0x20000]

	push esp
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax

	call interrupt_handler

	add esp, 4 ; Instead of pop'ing eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop esp

	add esp, 12

	iret

; Exceptions
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
interrupt 11
interrupt 12
interrupt 13
interrupt 14	
interrupt 15
; Non-exception interrups
interrupt 32
interrupt 33
interrupt 41
interrupt 49
interrupt 128
