org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

main:
	mov ax, 0
	mov ds, ax
	mov es, ax

	mov ss, ax
	mov sp, 0x7C00

	push es
	push word .after
	retf

.after:
	mov [drive_number], dl

	mov si, msg_loading
	call puts

	push es
	mov ah, 08h
	int 13h
	jc disk_error
	pop es

	mov ax, 9 ;0x01
	mov dl, [drive_number]
	mov cl, 32
	mov bx, 0x8E00
	call disk_read

	mov si, msg_done
	call puts

	jmp 0x8E00

	cli
	hlt

.halt:
	cli
	hlt

; --> si: Pointer to the string to print
puts:
	push si
	push ax
	push bx

.loop:
	lodsb
	or al, al
	jz .done

	mov ah, 0x0E
	int 0x10

	jmp .loop

.done:
	pop bx
	pop ax
	pop si
	ret

; --> ax: LBA address
; <-- cx bits 0 - 5: Sector
; <-- cx bits 6 - 15: Cylinder
; <-- dh: head
lba_to_chs:
	push ax
	push dx

	xor dx, dx
	div word [sectors_per_track]

	inc dx
	mov cx, dx

	xor dx, dx
	div word [heads]

	mov dh, dl
	mov ch, al
	shl ah, 6
	or cl, ah

	pop ax
	mov dl, al
	pop ax
	ret

; --> ax: LBA address
; --> cl: Number of sectors to read
; --> dl: Drive number
; --> es:bx Pointer where to store read data
disk_read:
    push ax
    push bx
    push cx
    push dx
    push di

    push cx
    call lba_to_chs
    pop ax
    
    mov ah, 02h
    mov di, 3

.retry:
    pusha
	stc
	int 13h
	jnc .done

    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry

.fail:
	jmp disk_error

.done:
    popa

    pop di
    pop dx
    pop cx
    pop bx
    pop ax
    ret

; --> dl: Drive number
disk_reset:
    pusha
    mov ah, 0
    stc
    int 13h
    jc disk_error
    popa
    ret

disk_error:
	mov si, msg_disk_error
	call puts
	ret

msg_loading: db "Loading...", ENDL, 0
msg_done: db "Entering stage 2", ENDL, 0
msg_disk_error: db "Disk error", ENDL, 0

sectors_per_track: dw 18
heads: dw 2

drive_number: db 0

times 510-($-$$) db 0
dw 0AA55h

buffer:
