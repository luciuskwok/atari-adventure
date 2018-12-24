; puff_asm.s


.export _bits_asm		; extern void __fastcall__ bits_asm(UInt8 need);
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

	ldy STATE_BITBUF		; keep bitbuf in Y?
	ldx #0
	lda #0
	beq while_need 	; do while (puff_state.bitcnt < need)

do_while:
	lda STATE_INCNT			; if STATE_INCNT == STATE_INLEN, out of input
	cmp STATE_INLEN
	beq error_jmp



	lda STATE_BITBUF		; load puff_state.bitbuf

	dey						; y -= 1
while_need:
	cpy #0
	bne do_while


							; returns bits in lowest part of value
	rts 					; return value is in A/X register (A=LSB, X=MSB)

error_jmp:
	lda     #<(STATE_ENV)	; longjmp(puff_state.env, 1);
	ldx     #>(STATE_ENV)
	jsr     pushax
	ldx     #$00
	lda     #$01
	jsr     _longjmp	
.endproc
