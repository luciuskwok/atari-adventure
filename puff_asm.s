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
	cmp #0
	bne set_up_locals
	ldx #0
	rts   					; if "need" == 0, return 0

set_up_locals:
	pha 					; save "need" for later
	tay						; use Y as decrementing index
	lda #0					; set return value = 0
	pha 					; up to 15 bytes are needed, so use 2 bytes on stack
	pha
	beq while_need 			; do while (need != 0)

do_need:		
	ldx STATE_INCNT			; if STATE_INCNT == STATE_INLEN:
	cpx STATE_INLEN 		;     error: out of input
	bne load_bitbuf;
	ldx STATE_INCNT+1		; must check both bytes of 16-bit values
	cpx STATE_INLEN+1
	beq error_jmp

load_bitbuf:
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
	tsx						; get the stack pointer
	lda $0101,X 			; load the next input byte (STATE_IN[STATE_INT])
	sta STATE_BITBUF		; store the byte in STATE_BITBUF
	lda #8
	sta STATE_BITCNT		; set STATE_BITCNT = 8
	pla						; pop the temp address off the stack
	pla

skip_load:					; DEFLATE format specifies that bits come off the right side
	lsr STATE_BITBUF		; shift right, 0->(bit 7), (bit 0)->carry
	tsx						; get the stack pointer
	ror $0101,X 			; MSB: carry->(bit 7), (bit 0)->carry
	ror $0102,X 			; LSB: carry->(bit 7), (bit 0)->carry(discarded)
	dec STATE_BITCNT		; STATE_BITCNT -= 1
	dey						; need -= 1
while_need:
	cpy #0					; while (need != 0)
	bne do_need

; At this point the return value is on the stack as two bytes, but the bits are left-justified
; and need to be right justified.
	tsx 					; get the stack pointer
	lda $0103,X 			; load "need" parameter (3rd byte on stack)
	sec 					; set carry so it doesn't affect subtraction
	sbc #16 				; Y = "need" - 16 = negative number of bit shifts
	tay 					; use Y as index register to count number of bit shifts
	clc
	bcc while_bit_shifter	; always branch

do_bit_shifter:
	lsr $0101,X 			; MSB: 0->(bit 7), (bit 0)->carry
	ror $0102,X 			; LSB: carry->(bit y), (bit 0)->carry(discarded)
	iny 					; Y += 1
while_bit_shifter:
	cpy #0 					; while (Y != 0)
	bne do_bit_shifter

return:
	pla	; MSB -> X
	tax 
	pla ; LSB -> Y
	tay
	pla ; discard to put stack pointer in correct place for return
	txa ; MSB -> A
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
