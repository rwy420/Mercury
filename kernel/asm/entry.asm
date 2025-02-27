extern kernel_main
global entry

entry:
	mov esp, 0x90000
	call kernel_main
