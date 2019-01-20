_first:
	call _second
	db "....WOODY....", 10

_second:
	pop rsi
	push rax
	push rdi
	push rdx
	mov rax, 0x1
	mov rdi, 0x1
	mov rdx, 0xe
	syscall
	pop rdx
	pop rdi
	pop rax
	jmp 0x0
