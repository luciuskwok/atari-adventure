; puff_asm.s


.export _bits_asm		; extern UInt16 __fastcall__ bits_asm(UInt8 need);
.import _puff_state
.import _longjmp
.import pushax

; puff_state struct fields
STATE_OUT = _puff_state
STATE_OUTLEN = _puff_state+2
STATE_OUTCNT = _puff_state+4
STATE_IN = _puff_state+6
STATE_INLEN = _puff_state+8
STATE_INCNT = _puff_state+10
STATE_BITBUF = _puff_state+12
STATE_BITCNT = _puff_state+13
STATE_ENV = _puff_state+14


.code

.proc _bits_asm				; fastcall: "need" parameter is passed in A. 
	tay						; use Y for need, which goes down to zero when complete
	lda #0					; set return value = 0
	pha 					; use 2 bytes on stack to locally store the 16-bit return value
	pha
	beq while_need 			; do while (need != 0)

do_while:		
	ldx STATE_INCNT			; if STATE_INCNT == STATE_INLEN:
	cpx STATE_INLEN 		;     error: out of input
	beq error_jmp

	ldx STATE_BITCNT		; if STATE_BITCNT == 0:
	bne skip_load			;     load new byte into STATE_BITBUF	

	clc						; add STATE_IN + STATE_INCNT
	lda STATE_IN
	adc STATE_INCNT
	tax
	lda STATE_IN+1
	adc STATE_INCNT+1
	pha 					; push result of addition onto stack to be used as address
	txa 
	pha
	tsx						; get the stack pointer, which is 1 byte below the previously pushed data
	inx
	lda $0100,X 			; load the byte at addr (STATE_IN[STATE_INT])
	sta STATE_BITBUF		; store the byte in STATE_BITBUF
	lda #8
	sta STATE_BITCNT		; set STATE_BITCNT = 8
	pla						; pop the temp address off the stack
	pla

skip_load:
	asl STATE_BITBUF		; shift left, bit 0 is set to 0, bit 7 is shifted into carry
	pla 					; load LSB
	rol a 					; rotate LSB left, carry into bit 0, bit 7 into carry
	tax 					; store LSB in X so it can be pushed last
	pla 					; load MSB
	rol a 					; rotate MSB left, carry into bit 0, bit 7 into carry (which is ignored)
	pha 					; push MSB
	txa
	pha 					; push LSB from X
	dec STATE_BITCNT		; STATE_BITCNT -= 1
	dey						; need -= 1
while_need:
	cpy #0					; while (need != 0)
	bne do_while

	pla						; pull LSB
	tay 					; move LSB to Y temporarily 
	pla						; pull MSB
	tax 					; move MSB to X for return
	tya 					; move LSB ot A for return
	rts 					; return bits

error_jmp:
	pla						; remove the 2 bytes that were reserved on the stack for the return value
	pla
	lda     #<(STATE_ENV)	; longjmp(puff_state.env, 1);
	ldx     #>(STATE_ENV)
	jsr     pushax
	ldx     #$00
	lda     #$01
	jsr     _longjmp	
.endproc
