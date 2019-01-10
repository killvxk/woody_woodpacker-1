	;;	nasm -f elf64 print.s -o print.o
	;;	ld -m elf_x86_64 print.o -o print

	;;  hexdump -v -e '"\\""x" 1/1 "%02x" ""' print

_first:
	jmp 0x0
	push rax					;baskup register
	push rdi
	push rdx
	call _second				; call permet de mettre l'addresse de str sur la stack
	db "surprise!", 10

_second:
	pop rsi 					; on recup l'addresse de str
	mov rax, 0x1					; syscall write
	mov rdi, 0x1					; arg1 stdout
	mov rdx, 0xa					; 10
	syscall
	pop rdx						;setback register
	pop rdi
	pop rax
