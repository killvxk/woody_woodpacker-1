_first:
	call _second
	db "....WOODY....", 10

_second:
	pop ecx						; truc to write
	push ebx
	push eax
	push edx
	mov ebx, 0x1				; fd
	mov eax, 0x4				;syscall
	mov edx, 0xe				; len
	int 0x80
	pop edx
	pop eax
	pop ebx
	jmp 0x0
