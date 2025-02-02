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

	/*mov ax, 9               ; Set up the disk read (e.g., LBA=9)
	mov dl, [drive_number]   ; Set the drive number
	mov cx, 50              ; Set CX to 100 (sectors to read)
	mov ax, 0x1000
	mov es, ax           ; Set SI to the buffer segment
	mov bx, 0
	;mov bx, 0x8e00

	call disk_read

	mov si, msg_done
	call puts

	;jmp 0x8e00
	jmp 0x1000:0x0000*/

	mov si, msg_disk_error
	call puts

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

print_hex:
    pusha                   ; Save all registers
    mov cx, 4               ; We have 4 hex digits (16-bit value)
    mov bx, si              ; BX holds the number to print

.hex_loop:
    rol bx, 4               ; Rotate left to get the next hex digit
    mov al, bl              ; Copy lowest 4 bits into AL
    and al, 0x0F            ; Mask out everything except the lowest nibble
    add al, '0'             ; Convert to ASCII ('0'-'9' or 'A'-'F')
    cmp al, '9'             ; If greater than '9', adjust for 'A'-'F'
    jbe .print
    add al, 7               ; Convert 'A'-'F'

.print:
    mov ah, 0x0E            ; BIOS teletype mode
    int 0x10                ; Print character

    loop .hex_loop          ; Repeat for all 4 hex digits

    ; Print newline (CR + LF)
    mov al, 0x0D            ; Carriage return (CR)
    int 0x10
    mov al, 0x0A            ; Line feed (LF)
    int 0x10

    popa                    ; Restore registers
    ret                     ; Return to caller

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
    call lba_to_chs          ; Convert LBA to CHS
    pop ax

	pusha
	mov si, ax
	call print_hex
	popa

    mov ah, 02h              ; BIOS read disk function
    mov di, 3                ; Retry 3 times

.retry:
    pusha
    stc
    int 13h                  ; Call BIOS disk read
    jnc .done                ; If successful, continue

    popa
    call disk_reset          ; Reset disk if failed
    dec di
    test di, di
    jnz .retry               ; Retry if attempts remain

.fail:
    jmp disk_error           ; Jump to error handler

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
