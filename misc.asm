; misc.asm


.import 	popptr1, popa
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4


; Zeropage locations
LMARGN = $52
RMARGN = $53
ROWCRS = $54
COLCRS = $55
SAVMSC = $58


.export _popStack
.proc _popStack
	; on entry: A=number of bytes to pop 
	; uses tmp1
	sta tmp1
	clc 
	lda sp 
	adc tmp1
	sta sp
	lda sp+1
	adc #0
	sta sp+1
	rts
.endproc 


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


; extern void __fastcall__ stringConcat(UInt8 *dst, const UInt8 *src);
.export _stringConcat 		
.proc _stringConcat
	; uses ptr1, ptr2
	; ptr1 is _stringLength parameter.
	SRC = ptr2
	sta SRC
	stx SRC+1
	DST = ptr1
	jsr popptr1

	jsr _stringLengthInternal ; DST += stringLength(DST)
	clc
	adc DST
	sta DST
	lda #0
	adc DST+1
	sta DST+1

	jsr _stringCopyInternal

	rts
.endproc


; extern void __fastcall__ stringCopy(UInt8 *dst, const UInt8 *src);
.export _stringCopy 	
.proc _stringCopy
	; uses ptr1, ptr2
	SRC = ptr2
	stx SRC+1
	sta SRC
	DST = ptr1
	jsr popptr1

	jsr _stringCopyInternal
	
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


; extern UInt8 __fastcall__ stringLength(UInt8 *s);
.export _stringLength 	
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


; extern UInt8 __fastcall__ toAtascii(UInt8 c);
.export _toAtascii
.proc _toAtascii
	cmp #$20			; if A < $20: 
	bcs mid_char
	adc #$40			; A += $40
	rts
mid_char:
	cmp #$60			; else if A < $60
	bcs return
	sec					; A -= $20
	sbc #$20
return:
	rts
.endproc


.export _multiplyAXtoPtr1
.proc _multiplyAXtoPtr1
	; Uses ptr2. Returns result in ptr1
	sta ptr2 			; ptr2 = A
	lda #0
	sta ptr2+1
	sta ptr1 
	sta ptr1+1
	jmp while
loop:
	txa  				; X >>= 1
	lsr a 
	tax 
	bcc shift_ptr2 		; if a bit fell off, add ptr2 to ptr1
add_ptr2:
	clc
	lda ptr1 
	adc ptr2 
	sta ptr1 
	lda ptr1+1  		; add any carry to MSB
	adc ptr2+1
	sta ptr1+1
shift_ptr2:
	asl ptr2 
	rol ptr2+1
while:
	cpx #0
	bne loop
	rts
.endproc


; extern void __fastcall__ printLine(UInt8 *s);
.export _printLine
.proc _printLine
	; uses ptr1, ptr2, AY
	cpx #0
	beq next_line		; if s == NULL: jump to next line

	pha  				; push s onto stack
	txa 
	pha 

	lda ROWCRS			; ptr1 = *ROWCRS
	ldx #40
	jsr _multiplyAXtoPtr1

	clc 				; ptr1 += *SAVMSC
	lda ptr1
	adc SAVMSC
	sta ptr1
	lda ptr1+1
	adc SAVMSC+1
	sta ptr1+1			; ptr1 now points at beginning of screen row

	clc 				; ptr1 += *COLCRS
	lda ptr1
	adc COLCRS
	sta ptr1
	lda ptr1+1
	adc #0
	sta ptr1+1			; ptr1 now points at screen cell to start at

	pla  				; ptr2 = s
	sta ptr2+1
	pla 
	sta ptr2 

	ldy #0
loop:
	lda(ptr2),Y
	beq next_line		; if s[Y] == 0: jump to next line
	jsr _toAtascii
	sta(ptr1),Y			; screen[Y] = s[Y]
	iny
	bne loop
next_line:
	inc ROWCRS
	lda LMARGN
	sta COLCRS
	lda #0
	sta COLCRS+1
	rts
.endproc
