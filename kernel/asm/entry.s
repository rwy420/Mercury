.extern kernel_main

.global entry
.global stack

.section .entry
entry:
	# Set stack pointer
	mov $0x90000, %esp

	call kernel_main

# Should not get to this point
_halt:
	cli
	hlt
	jmp _halt

#.section .bss
#.space 4*1024*1024 
#stack:
