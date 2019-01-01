; misc_asm.s

.export _stringConcat 		
; extern void __fastcall__ stringConcat(UInt8 *dst, const UInt8 *src);

.export _stringCopy 	
; extern void __fastcall__ stringCopy(UInt8 *dst, const UInt8 *src);

.export _stringLength 	
; extern UInt8 __fastcall__ stringLength(UInt8 *s);

.export 	_decodeRunLenRange 
; extern void __fastcall__ decodeRunLenRange(UInt8 *outData, UInt8 skip, UInt8 length, const UInt8 *runLenData);

.import 	pushax
.import		incsp2, incsp4
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4


.proc _decodeRunLenRange
.code
IN = ptr1				; $8A
	sta IN
	stx IN+1

IN_LEN = tmp1			; $92
	ldy #0
	lda (IN),Y
	sta IN_LEN

IN_INDEX = tmp2			; $93
	lda #1
	sta IN_INDEX

OUT = ptr2				; $8C
	ldy #2
	lda (sp),Y
	sta OUT
	ldy #3
	lda (sp),Y
	sta OUT+1

SKIP = ptr4				; $90
	ldy #1
	lda (sp),Y
	sta SKIP

OUT_LEN = tmp3			; $94
	ldy #0
	lda (sp),Y
	sta OUT_LEN

OUT_INDEX = tmp4		; $95
	lda #0
	sta OUT_INDEX

TILE = ptr3				; $8E
REPEAT = ptr3+1			; $8F
	
	jmp while_input 	; 

loop_input:
	ldy IN_INDEX	 	; A = in[in_index++]
	lda (IN),Y
	iny
	tax

	and #$F0			; tile = top 4 bits of A
	lsr a
	lsr a
	lsr a
	lsr a
	sta TILE

	txa
	and #$0F 			; repeat = bottom 4 bits of A
	cmp #$0F
	bne store_repeat 	; if repeat=$F, get add next byte

	clc
	adc (IN),Y			; repeat += in[in_index++]
	iny

store_repeat:
	clc
	adc #1
	sta REPEAT
	sty IN_INDEX
	
	ldy OUT_INDEX		; use Y as out_index within the output loop
	jmp while_output

loop_output:
	lda SKIP			; if skip >= repeat
	cmp REPEAT
	bcc check_skip
	
	sbc REPEAT 			; fast-forward past skipped bytes
	sta SKIP
	jmp while_input		; and continue to next input byte

check_skip:
	dec REPEAT			; repeat -= 1

	ldx SKIP			; if skip > 0
	beq output_byte
	dex					; skip -= 1
	stx SKIP
	jmp while_output

output_byte:
	lda TILE
	sta (OUT),Y
	iny

while_output: 			; while (repeat > 0 && outIndex < end)
	lda REPEAT
	beq while_input
	
	cpy OUT_LEN			; Y = out_index
	bcs while_input

	jmp loop_output

while_input:			; while (in_index < in_len && out_index < out_len)
	sty OUT_INDEX		; save Y in out_index

	lda IN_INDEX		; check in_index < in_len
	cmp IN_LEN
	bcs return

	lda OUT_INDEX		; check out_index < out_len
	cmp OUT_LEN
	bcs return

	jmp loop_input

return:
	jsr incsp4
	rts
.endproc

.proc _stringConcat
; uses ptr1, ptr2
; ptr1 is _stringLength parameter.
SRC = ptr2
	sta SRC
	stx SRC+1
DST = ptr1
	ldy #1				; MSB
	lda (sp),Y
	sta DST+1
	ldy #0				; LSB
	lda (sp),Y
	sta DST

	jsr _stringLengthInternal ; DST += stringLength(DST)
	clc
	adc DST
	sta DST
	lda #0
	adc DST+1
	sta DST+1

	jsr _stringCopyInternal
	jsr incsp2
	rts
.endproc

.proc _stringCopy
; uses ptr1, ptr2
SRC = ptr2
	stx SRC+1
	sta SRC
DST = ptr1
	ldy #1				; MSB
	lda (sp),Y
	sta DST+1
	ldy #0				; LSB
	lda (sp),Y
	sta DST

	jsr _stringCopyInternal
	jsr incsp2
	rts
.endproc

.proc _stringCopyInternal
; Copy null-terminated string from ptr2 to ptr1.
; On input, ptr1 is destination, ptr2 is source.
	ldy #0
loop:
	lda (ptr2),Y
	sta (ptr1),Y
	iny
	beq return
	cmp #0
	bne loop
return:
	rts
.endproc

.proc _stringLength
; uses ptr1
STR = ptr1
	sta STR
	stx STR+1
	jsr _stringLengthInternal
	rts
.endproc

.proc _stringLengthInternal
; On input, ptr1 points to string
STR = ptr1
	ldy #0
loop:
	lda (STR),Y
	beq return
	iny
	bne loop
return:
	tya
	ldx #0
	rts
.endproc
