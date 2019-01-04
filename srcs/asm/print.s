	;;	nasm -f elf64 print.s -o print.o
	;;	ld -m elf_x86_64 print.o -o print

	;;  hexdump -v -e '"\\""x" 1/1 "%02x" ""' print

	global	_start
	section _text

_start:
	jmp _first

_first:
	call _second				; call permet de mettre l'addresse de str sur la stack
    db "Hello, World!", 10

_second:
	mov rax, 0x4
	mov rbx, 0x1 				; read
	pop rcx 					; on recup l'addresse de str
	mov rdi, 0x1				; stdout
	mov rdx, 0xe				; 14
	int 0x80					; syscall

	ret
