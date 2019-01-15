; misc.asm


.code

.export _multiplyAXtoPtr1
.proc _multiplyAXtoPtr1
	; Uses sreg. Returns result in ptr1
	.importzp sreg, ptr1 

	sta sreg 				; sreg = A
	lda #0	
	sta sreg+1
	sta ptr1 
	sta ptr1+1
	jmp while
	loop:
		txa  				; X >>= 1
		lsr a 
		tax 
		bcc shift_sreg 		; if a bit fell off, add sreg to ptr1
	add_sreg:
		jsr _addSregToPtr1
	shift_sreg:
		asl sreg 
		rol sreg+1
	while:
		cpx #0
		bne loop
	rts
.endproc

.export _addSregToPtr1
.proc _addSregToPtr1
	.importzp sreg, ptr1 

	clc 
	lda ptr1 
	adc sreg 
	sta ptr1 
	lda ptr1+1
	adc sreg+1
	sta ptr1+1
	rts 
.endproc 

.export _addAToPtr1
.proc _addAToPtr1
	.importzp ptr1 

	clc 
	adc ptr1 
	sta ptr1 
	bcc return 
		inc ptr1+1
	return:
	rts 
.endproc 

.export _addAToPtr2
.proc _addAToPtr2
	.importzp ptr2 

	clc 
	adc ptr2
	sta ptr2 
	bcc return 
		inc ptr2+1
	return:
	rts 
.endproc 
