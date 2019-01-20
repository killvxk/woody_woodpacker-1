entry:
	push rax
	push rdi
	push rsi
	push rdx
	push r8
	push r9

	xor	rsi, rsi
	xor	rax, rax
	lea	r8, [ decrypt ]	; begin encrypted address
	mov	eax, 0x42				; size
	mov bl, 0x42
	call decrypt

decrypt:
	cmp	eax, 0					; count == 0 ?
	je	init_payload
	xor rdx, rdx
	mov	dl, byte [ r8 ]			; data[i]
	xor dl, bl					;data[i] ^ key[i]
	mov byte[r8], dl			;data[i] = result

	dec	eax
	inc r8
	jmp	decrypt

init_payload:
	call	payload
	db		"...WOODY...", 0xa

payload:
	mov	rdi, 0x1
	pop	rsi
	mov	rdx, 0xc
	mov	rax, 0x1
	syscall						; write
	pop	r9
	pop	r8
	pop	rdx
	pop	rsi
	pop	rdi
	pop	rax
	jmp	0xcafebabe				; original entrypoint
