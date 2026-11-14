section .text

global _start
extern main

_start:
	xor ebp, ebp
	mov eax, [esp] ; int argc
	lea ebx, [esp + 4] ; char** argv

	push ebx
	push eax
	call main

	; Exit syscall
	mov ebx, eax ; use return value from main as error code
	mov eax, 1 ; syscall 1 = exit
	int 0x80

	; Shouldn't get here
	ud2 ; ud2 will cause an invalid opcode instruction which terminates the task and therefore is the final way out of a task in case the
		; syscall to exit failed somehow
