; puff_asm.s


.export 	_decode_asm ; extern UInt16 __fastcall__ bits_decode(const struct huffman *h);
.export 	_bits_asm ; extern UInt16 __fastcall__ bits_asm(UInt8 count);
.export 	_get_one_bit ; extern UInt16 __fastcall__ get_one_bit(void);
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

; constants
MAXBITS = 15


.code

.proc _decode_asm
	sta ptr1 				; ptr1 = h
	stx ptr1+1
	ldy #0 					; ptr2 = (UInt8*) h->count
	lda (ptr1),Y 			; this is incremented instead of using var len
	sta ptr2				; because: count = h->count[len]
	iny
	lda (ptr1),Y
	sta ptr2+1
	iny 					; ptr3 = (Uint16*) h->symbol
	lda (ptr1),Y 			; 
	sta ptr3 				; 
	iny
	lda (ptr1),Y
	sta ptr3+1
							; Local Variables
LEN = $0108
	lda #MAXBITS 			; var len:UInt8 = 15
	pha 					; sp+$0108
COUNT = $0107
	lda #0					; var count:UInt8 = 0 // number of codes of length len
	pha 	 				; 
CODE = $0105
	pha 					; var code:UInt16 = 0 // len bits being decoded
	pha  					; 
FIRST = $0103
	pha 					; var first:UInt16 = 0 // first code of length len
	pha 					; sp+$0103
INDEX = $0101
	pha 					; var index:UInt16 = 0 // index of first code of length len in symbol table
	pha 					; sp+$0101
	tsx 					; X = stack pointer
	jmp while_loop

do_loop:
	inc ptr2 				; ptr2 += 1 
	bne load_byte 			;   (was len++ and h->count[len])
	inc ptr2+1

load_byte:
	lda STATE_BITCNT 	; if bitcnt == 0: load byte
	bne get_bit

	clc				; ptr4 = STATE_IN + STATE_INCNT
	lda STATE_IN
	adc STATE_INCNT
	sta ptr4
	lda STATE_IN+1
	adc STATE_INCNT+1
	sta ptr4+1
	ldy #0
	lda (ptr4),Y	; A = *ptr4, the next input byte
	sta STATE_BITBUF ; store the byte in STATE_BITBUF

	lda #8 			; STATE_BITCNT = 8
	sta STATE_BITCNT		

	inc STATE_INCNT ; STATE_INCNT += 1
	bne get_bit
	inc STATE_INCNT+1

get_bit:
	; STATE_BITCNT -= 1
	dec STATE_BITCNT

	; code = (code << 1) | (bitbuf & 1); bitbuf >>= 1
	lsr STATE_BITBUF ; bitbuf: bit0->carry
	rol CODE,X 		; code.LSB: carry->bit0, bit7->carry
	rol CODE+1,X 	; code.MSB

	; count = *ptr2 (was count = h->count[len])
	ldy #0
	lda (ptr2),Y
	sta COUNT,X

	; if (code < first + count) aka (code - (first + count) < 0)
	clc 			; tmp = first + count
	lda FIRST,X 	; tmp1 = first.LSB + count
	adc COUNT,X
	sta tmp1
	lda FIRST+1,X 	; tmp2 = carry + first.MSB
	adc #0
	sta tmp2
	sec 			; subtract tmp from code
	lda CODE,X
	sbc tmp1 		; rearranging the terms: (code < first + count) :: (code - (first + count) < 0)
	lda CODE+1,X
	sbc tmp2
	bcc return_symbol ; if result < 0 (C=0): return symbol

	clc 			; first = tmp 
	lda tmp1 		;   (was first = first + count)
	sta FIRST,X
	lda tmp2
	sta FIRST+1,X

	clc 			; index += count
	lda INDEX,X 	;   index.LSB += count
	adc COUNT,X
	sta INDEX,X
	lda INDEX+1,X 	;   index.MSB += carry
	adc #0
	sta INDEX+1,X

	asl FIRST,X 	; first <<= 1
	rol FIRST+1,X

	asl CODE,X 		; code <<= 1
	rol CODE+1,X

decrement_len:
	dec LEN,X 		; len -= 1		

while_loop:
	lda LEN,X
	beq return_error ; while len != 0
	jmp do_loop

return_symbol:
	clc 			; tmp = index + code
	lda INDEX,X
	adc INDEX+1,X
	sta tmp1
	lda CODE,X
	adc CODE+1,X
	sta tmp2

	sec  			; tmp -= first
	lda tmp1
	sbc FIRST,X
	sta tmp1
	lda tmp2
	sbc FIRST+1,X
	sta tmp2
	
	asl tmp1 		; tmp *= 2
	rol tmp2,X

	clc 			; ptr3 += tmp
	lda ptr3
	adc tmp1
	sta ptr3
	lda ptr3+1
	adc tmp2
	sta ptr3+1

	tsx 	; pop 8 bytes off stack to remove local vars
	txa 	; 2
	clc 	; 2
	adc #8 	; 2
	tax 	; 2
	txs 	; 2 cycles
		
	ldy #1 			; return *ptr3 
	lda (ptr3),Y 	;    (was h->symbol[index + (code - first)])
	tax
	ldy #0
	lda (ptr3),Y
	rts

return_error:
	tsx 	; 2 ; pop local vars
	txa 	; 2 ; this is faster than using pla which is 4 cycles each instruction
	clc 	; 2
	adc #8 	; 2
	tax 	; 2
	txs 	; 2 cycles

	lda #$F6 ; return -10
	ldx #$FF 
	rts
.endproc

.proc _bits_asm
	sta tmp1				; let tmp1 = count
	lda #0					; 
	sta tmp2				; var tmp2/tmp3: return result
	sta tmp3
	ldy #0					; var index = 0
	beq while_count

do_count:
	lsr tmp3 				; tmp.MSB >>= 1
	ror tmp2 				; tmp.LSB >>= 1 with carry

	cpy tmp1				; if index < count:
	bcs continue_count

	jsr _get_one_bit		; A = get_one_bit()
	cmp #0
	beq continue_count		; if A != 0:
	lda #$80				; set high bit of tmp2
	ora tmp3
	sta tmp3

continue_count:
	iny						; index += 1

while_count:
	cpy #16					; while index != 16
	bne do_count

return:
	lda tmp2				; LSB -> A
	ldx tmp3 				; MSB -> X
	rts 					; return bits

.endproc

.proc _get_one_bit			; Returns 0 or 1 in the A/X register
							; Uses ptr1, A, X. Does not use Y.
	ldx #0 					; X is always 0
	lda STATE_BITCNT		; if STATE_BITCNT == 0:
	bne get_bit				;     load new byte into STATE_BITBUF	

	lda STATE_INCNT			; if STATE_INCNT == STATE_INLEN:
	cmp STATE_INLEN
	bne load_bitbuf
	lda STATE_INCNT+1		; must check both bytes of 16-bit values
	cmp STATE_INLEN+1
	bne load_bitbuf

	lda #<(STATE_ENV) 		; error: out of input
	ldx #>(STATE_ENV)
	jsr pushax
	ldx #$00
	lda #$01
	jsr _longjmp			; longjmp(puff_state.env, 1);

load_bitbuf:
	clc						; ptr1 = STATE_IN + STATE_INCNT
	lda STATE_IN
	adc STATE_INCNT
	sta ptr1
	lda STATE_IN+1
	adc STATE_INCNT+1
	sta ptr1+1
	lda (ptr1,X)			; A = *ptr1, the next input byte
	sta STATE_BITBUF		; store the byte in STATE_BITBUF

	lda #8 					; STATE_BITCNT = 8
	sta STATE_BITCNT		

	inc STATE_INCNT 		; STATE_INCNT += 1
	bne get_bit
	inc STATE_INCNT+1

get_bit:					; DEFLATE specifies that bits come off the right
	dec STATE_BITCNT		; STATE_BITCNT -= 1
	lda #0					; X was always 0
	lsr STATE_BITBUF		; shift right, 0->(bit 7), (bit 0)->carry
	rol a 					; put bit into A
	rts 					; return bits

.endproc
