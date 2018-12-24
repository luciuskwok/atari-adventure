; puff_asm.s


.export 	_bits_asm ; extern UInt16 __fastcall__ bits_asm(UInt8 count);
.import 	_puff_state
.import 	_longjmp
.import 	pushax
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

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

.proc _bits_asm
	cmp #0					; count parameter is passed in A. 
	bne set_up_locals		; if count == 0, return 0
	ldx #0
	rts   			

set_up_locals:
	tay						; count -> Y, used as decrementing index
	lda #0					; local var result = UInt16(0)
	pha 					; 
	pha 					; reserve 2 bytes, because count <= 15
	tya
	pha 					; save count parameter for later
	bne while_need 			; do while (Y != 0)

do_need:		
	lda STATE_BITCNT		; if STATE_BITCNT == 0:
	bne get_bit				;     load new byte into STATE_BITBUF	

	lda STATE_INCNT			; if STATE_INCNT == STATE_INLEN:
	cmp STATE_INLEN 		;     error: out of input
	bne load_bitbuf
	lda STATE_INCNT+1		; must check both bytes of 16-bit values
	cmp STATE_INLEN+1
	beq error_jmp

load_bitbuf:
	clc						; ptr1 = STATE_IN + STATE_INCNT
	lda STATE_IN
	adc STATE_INCNT
	sta ptr1
	lda STATE_IN+1
	adc STATE_INCNT+1
	sta ptr1+1
	ldx #0
	lda (ptr1,X)			; A = *ptr1, the next input byte
	sta STATE_BITBUF		; store the byte in STATE_BITBUF

	lda #8
	sta STATE_BITCNT		; set STATE_BITCNT = 8

	inc STATE_INCNT 		; ++STATE_INCNT
	bne get_bit
	inc STATE_INCNT+1

get_bit:					; DEFLATE format specifies that bits come off the right side
	lsr STATE_BITBUF		; shift right, 0->(bit 7), (bit 0)->carry
	tsx						; get the stack pointer
	ror $0102,X 			; MSB: carry->(bit 7), (bit 0)->carry
	ror $0103,X 			; LSB: carry->(bit 7), (bit 0)->carry(discarded)
	dec STATE_BITCNT		; STATE_BITCNT -= 1
	dey						; Y -= 1
while_need:
	cpy #0					; while (Y != 0)
	bne do_need

; At this point the return value is on the stack as two bytes, but the bits are left-justified
; and need to be right justified.
	tsx
	lda $0101,X 			; load count
	sec 					; set carry so it doesn't affect subtraction
	sbc #16 				; Y = count - 16 = negative number of bit shifts
	tay 					; use Y as index register to count number of bit shifts
	clc
	bcc while_bit_shifter	; always branch

do_bit_shifter:
	lsr $0102,X 			; MSB: 0->(bit 7), (bit 0)->carry
	ror $0103,X 			; LSB: carry->(bit y), (bit 0)->carry(discarded)
	iny 					; Y += 1
while_bit_shifter:
	cpy #0 					; while (Y != 0)
	bne do_bit_shifter

return:
	pla ; count
	pla	; MSB -> X
	tax 
	pla ; LSB -> A
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
