.extern kernel_main

.global entry
.global stack

.section .entry
entry:
	mov $0x90000, %esp

	call kernel_main

_halt:
	cli
	hlt
	jmp _halt
