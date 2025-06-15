.set ALIGN, 1<<0
.set MEMINFO, 1<<1
.set VIDEO_MODE, 1<<2
.set FLAGS, ALIGN | MEMINFO | VIDEO_MODE
.set MAGIC, 0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot.data, "wa"
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0          # header_addr (can be 0 if unused)
.long 0          # load_addr
.long 0          # load_end_addr
.long 0          # bss_end_addr
.long 0          # entry_addr
.long 0          # mode_type (0 = linear graphics mode)
.long 1024       # width
.long 768        # height
.long 32         # depth (bits per pixel)

.section .multiboot.text, "wa"
.global _start
.type _start, @function
_start:
	#xchg %bx, %bx
	mov $0x8FFF0, %esp
	mov $0x1017ad, %eax
	call %eax

	cli
1:	hlt
	jmp 1b

.size _start, . - _start
