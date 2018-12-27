; map_asm.s

.export 	_decodeRunLenRange 
; extern void __fastcall__ decodeRunLenRange(UInt8 *outData, UInt8 skip, UInt8 length, const UInt8 *runLenData);

.import 	pushax
.import		incsp2, incsp4
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4


.proc _decodeRunLenRange
.code
OUT = ptr2
	sta OUT
	stx OUT+1

SKIP = ptr4
	ldy #0
	lda (sp),Y
	sta SKIP

OUT_LEN = tmp3
	ldy #1
	lda (sp),Y
	sta OUT_LEN

OUT_INDEX = tmp4
	lda #0
	sta OUT_INDEX

IN = ptr1
	ldy #2
	lda (sp),Y
	sta IN
	ldy #3
	lda (sp),Y
	sta IN+1

IN_LEN = tmp1
	ldy #0
	sta IN_LEN

IN_INDEX = tmp2
	lda #1
	sta IN_INDEX

TILE = ptr3
REPEAT = ptr3+1
	
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
	
	jmp while_output

loop_output:
	dec REPEAT			; repeat -= 1

	ldx SKIP			; if skip > 0
	beq output_byte
	dex					; skip -= 1
	stx SKIP
	jmp while_output

output_byte:
	ldy OUT_INDEX 		; out[out_index++] = tile
	lda TILE
	sta (OUT),Y
	iny
	sty OUT_INDEX

while_output: 			; while (repeat > 0 && outIndex < end)
	lda REPEAT
	beq while_input
	
	lda OUT_INDEX
	cmp OUT_LEN
	bcs while_input

	jmp loop_output

while_input:			; while (in_index < in_len && out_index < out_len)
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

