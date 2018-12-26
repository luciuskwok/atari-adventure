; puff_asm.s


.export 	_codes_asm ; extern SInt16 __fastcall__ codes_asm(const struct huffman *lencode, const struct huffman *distcode);
.export 	_decode_asm ; extern UInt16 __fastcall__ decode_asm(const struct huffman *h);
.export 	_bits_asm ; extern UInt16 __fastcall__ bits_asm(UInt8 count);
.export 	_get_one_bit ; extern UInt16 __fastcall__ get_one_bit(void);
.import 	_puff_state
.import 	_longjmp
.import 	pushax
.import		incsp4
.importzp 	sp, sreg
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
MAXBITS   = 15
MAXLCODES = 286				; maximum number of literal/length codes
MAXDCODES = 30            	; maximum number of distance codes
MAXCODES  = MAXLCODES+MAXDCODES  ; maximum codes lengths to read
FIXLCODES = 288           	; number of fixed literal/length codes


.proc _codes_asm			; returns error code or 0 on A/X
							; uses all ptr and tmp registers

.rodata
LEN_BASES:
	.word $0003, $0004, $0005, $0006, $0007, $0008, $0009, $000A, $000B, $000D, $000F, $0011, $0013, $0017, $001B, $001F, $0023, $002B, $0033, $003B, $0043, $0053, $0063, $0073, $0083, $00A3, $00C3, $00E3, $0102
LEN_EXTRA_BITS:
	.byte $00, $00, $00, $00, $00, $00, $00, $00, $01, $01, $01, $01, $02, $02, $02, $02, $03, $03, $03, $03, $04, $04, $04, $04, $05, $05, $05, $05, $00
DIST_BASES:
	.word $0001, $0002, $0003, $0004, $0005, $0007, $0009, $000D, $0011, $0019, $0021, $0031, $0041, $0061, $0081, $00C1, $0101, $0181, $0201, $0301, $0401, $0601, $0801, $0C01, $1001, $1801, $2001, $3001, $4001, $6001
DIST_EXTRA_BITS:
	.byte $00, $00, $00, $00, $01, $01, $02, $02, $03, $03, $04, $04, $05, $05, $06, $06, $07, $07, $08, $08, $09, $09, $0A, $0A, $0B, $0B, $0C, $0C, $0D, $0D

.code
; parameters: *lencode is on parameter stack, *distcode is on A/X.
	jsr pushax				; push distcode onto parameter stack
DISTCODE = $00
LENCODE = $02 				; on parameter stack
LEN = ptr1
DIST = ptr2
SYMBOL = ptr3

loop:
	; symbol = decode_asm(lencode);
	ldy LENCODE+1 			; get lencode from parameter stack
	lda (sp),Y
	tax
	dey
	lda (sp),Y
	jsr _decode_asm 		; AX = decode_asm(lencode)

	jsr _validate_symbol 	; sets carry if symbol is invalid.
	bcc error_invalid_symbol

	sta SYMBOL 				; symbol = AX
	stx SYMBOL+1

	; if (symbol < 256) // literal: symbol is the byte
	cpx #0
	bne length_symbol

	jsr write_out_symbol





length_symbol:



continue_loop:
	lda #$00
	cmp SYMBOL
	bne loop
	lda #$01
	cmp SYMBOL+1
	bne loop

return_zero:
	jsr incsp4
	ldx #0
	lda #0
	rts

error_distance_too_far:
	jsr incsp4
	ldx #$FF
	lda #$F5
	rts

error_invalid_symbol:
	jsr incsp4
	ldx #$FF
	lda #$F6
	rts
.endproc


.proc _write_out_symbol 	; adds byte in A to puff_state.out
.code						; uses ptr1, X, Y
	tax 					; save A in X

	lda STATE_INCNT			; if STATE_OUTCNT == STATE_OUTLEN: output is full
	cmp STATE_INLEN
	bne load_bitbuf
	lda STATE_INCNT+1		; must check both bytes of 16-bit values
	cmp STATE_INLEN+1
	bne load_bitbuf



	tax
	ldy #0
	sta (ptr1),Y

	lda #0 					; return 0 for success
	rts
.endproc


.proc _validate_symbol 		; sets carry flag if symbol in AX is invalid
.code
	cpx #>(MAXCODES)		; carry flag is set if MSB in X is >= maxcodes.msb
	beq check_symbol_lsb
	rts 
check_symbol_lsb:
	cmp #<(MAXCODES)
	rts
.endproc


.proc _decode_asm			; returns symbol or error -10 ($F6)
.code						; uses AXY & all ptr registers & tmp1, tmp3, tmp4 registers.
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
COUNT = tmp1				; // number of codes of length len
	lda #0
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
	cmp #1				; 2 ; sets carry if A >= 1
	rol CODE			; 5 ; code.LSB
	rol CODE+1			; 5 ; code.MSB

	; A = count = h->count[len]
	lda (H_COUNT),Y		; 5 ;
	sta COUNT 			; 3

	; if (code < first + count) aka (code - (first + count) < 0)
	clc 				; 2 ; XA = first + count
	adc FIRST	 		; 3 ; X = first.LSB + count
	tax			 		; 2 ;
	lda #0 				; 2 ; A = carry + first.MSB
	adc FIRST+1 		; 3 ; 

	; if ptr1 > code: return symbol
	cmp CODE+1			; 3 ; if A > code.MSB, C=0, Z=0
	beq compare_lsb		; 2/3
	bcs return_symbol  	; 2

compare_lsb:
	cpx CODE			; 3 ; if X > code.MSB, C=0, Z=0
	beq continue_loop	; 3
	bcs return_symbol	; 3

continue_loop:
	stx FIRST			; 3 ; first = XA
	sta FIRST+1			; 3 ;

	asl FIRST 			; 5 ; first <<= 1
	rol FIRST+1			; 5 ; 

	asl COUNT 			; 5 ; count *= 2

	clc 					; 2 ; h_symbol_index += count
	lda H_SYMBOL_INDEX  	; 3 ; index.LSB += count
	adc COUNT	 			; 3 ; 
	sta H_SYMBOL_INDEX		; 3 ; 
	lda H_SYMBOL_INDEX+1 	; 3 ; index.MSB += carry
	adc #0	 				; 2 ; 
	sta H_SYMBOL_INDEX+1	; 3 ; 

	iny						; 2 ; len += 1

while_loop:
	cpy #MAXBITS+1 			; 2 ; if Y <= MAXBITS
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
	lsr STATE_BITBUF		; shift right, 0->(bit 7), (bit 0)->carry
	lda #0					; X was always 0
	rol a 					; put carry bit into A
	rts 					; return bits

.endproc
