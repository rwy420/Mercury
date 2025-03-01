extern kernel_main
global k_entry

k_entry:
	mov esp, 0x90000
	call kernel_main
