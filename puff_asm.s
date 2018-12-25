; puff_asm.s


.export 	_decode_asm ; extern UInt16 __fastcall__ bits_decode(const struct huffman *h);
.export 	_bits_asm ; extern UInt16 __fastcall__ bits_asm(UInt8 count);
.export 	_get_one_bit ; extern UInt16 __fastcall__ get_one_bit(void);
.import 	_puff_state
.import 	_longjmp
.import 	pushax
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
.importzp 	sreg

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
	sta ptr1 				; ptr1 = h, temporarily
	stx ptr1+1
H_COUNT = ptr2
	ldy #0 					; ptr2 = (UInt8*) h->count
	lda (ptr1),Y 			
	sta H_COUNT			
	iny
	lda (ptr1),Y
	sta H_COUNT+1
H_SYMBOL_INDEX = ptr3
	iny 					; ptr3 = (Uint16*) h->symbol
	lda (ptr1),Y			;     plus index * 2
	sta H_SYMBOL_INDEX
	iny
	lda (ptr1),Y
	sta H_SYMBOL_INDEX+1
COUNT = tmp1
	lda #0					; var count:UInt8 = 0 
	sta COUNT 				; // number of codes of length len
CODE = tmp3
	sta CODE 				; var code:UInt16 = 0 
	sta CODE+1 				; // len bits being decoded
FIRST = ptr4
	sta FIRST 				; var first:UInt16 = 0 
	sta FIRST+1				; // first code of length len

	ldy #1					; for len in 1...15 (use Y for len)
	jmp while_loop

do_loop:
	jsr _get_one_bit		; A = _get_one_bit()

	; code = code << 1 | A
	asl CODE 		; code.LSB: carry->bit0, bit7->carry
	rol CODE+1	 	; code.MSB
	ora CODE
	sta CODE

	; A = count = h->count[len]
	lda (H_COUNT),Y
	sta COUNT

	; if (code < first + count) aka (code - (first + count) < 0)
	clc 			; ptr1 = first + count
	adc FIRST	 	; ptr1.LSB = first.LSB + count
	sta ptr1
	adc FIRST+1 	; ptr1.MSB = carry + first.MSB
	sta ptr1+1

	sec 			; if code < tmp(3,4)
	lda CODE
	cmp ptr1
	lda CODE+1
	cmp ptr1+1
	bcc return_symbol ; if result < 0 (C=0): return symbol

	clc 			; first = tmp(3,4)
	lda ptr1 		;   (was first = first + count)
	sta FIRST
	lda ptr1+1
	sta FIRST+1

	asl FIRST 		; first <<= 1
	rol FIRST+1

	asl COUNT 		; count *= 2

	clc 			; h_symbol_index += count
	lda H_SYMBOL_INDEX 		; index.LSB += count
	adc COUNT
	sta H_SYMBOL_INDEX
	lda H_SYMBOL_INDEX+1 	; index.MSB += carry
	adc #0
	sta H_SYMBOL_INDEX+1

continue_loop:
	iny						; len += 1

while_loop:
	cpy #MAXBITS
	bne do_loop

return_error:
	lda #$F6 				; return -10
	ldx #$FF 
	rts

return_symbol:
	sec  					; code -= first
	lda CODE
	sbc FIRST
	sta CODE
	lda CODE+1
	sbc FIRST+1
	sta CODE+1
	
	asl CODE 				; code *= 2
	rol CODE+1

	clc 					; h_symbol_index += code
	lda H_SYMBOL_INDEX
	adc CODE
	sta H_SYMBOL_INDEX
	lda H_SYMBOL_INDEX+1
	adc CODE+1
	sta H_SYMBOL_INDEX+1

	ldy #1 					; return h->symbol[index + code - first]
	lda (H_SYMBOL_INDEX),Y
	tax
	ldy #0
	lda (H_SYMBOL_INDEX),Y
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
