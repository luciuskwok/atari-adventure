; misc.asm


.import 	popptr1, popa
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4


; extern void __fastcall__ decodeRunLenRange(UInt8 *outData, UInt8 skip, UInt8 length, const UInt8 *runLenData);
.export _decodeRunLenRange 
.proc _decodeRunLenRange
.code
IN = ptr2				; $8A
	sta IN
	stx IN+1

IN_LEN = tmp1			; $92
	ldy #0
	lda (IN),Y
	sta IN_LEN

IN_INDEX = tmp2			; $93
	lda #1
	sta IN_INDEX

OUT_LEN = tmp3			; $94
	jsr popa
	sta OUT_LEN

SKIP = ptr4				; $90
	jsr popa 
	sta SKIP

OUT = ptr1				; $8C
	jsr popptr1

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
	rts
.endproc
