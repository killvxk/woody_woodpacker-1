entry:
	push    rax
	push    rdi
	push    rsi
	push    rdx
	push    r8

init_payload:
	call    payload
	db        "...WOODY...", 0xa

payload:
	mov        rdi, 0x1
	pop        rsi
	mov        rdx, 0xc
	mov        rax, 0x1
	syscall

init_decrypter:
	xor        r8, r8
	xor        rax, rax

	mov        eax, 0x42		; data len
	lea        r8, [ rel loop ] ; crypted data

loop:
	cmp	    	eax, 0x0
	je			end_decrypter

	mov        dl, byte[r8]
	xor        dl, 0x61
	mov        byte[r8], dl

	dec        eax
	inc        r8

	jmp loop

end_decrypter:
	pop        r8
	pop        rdx
	pop        rsi
	pop        rdi
	pop        rax
	jmp        0xcafebabe
