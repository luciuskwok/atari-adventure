; text_.asm

.import 	popptr1, popa
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

; Zeropage locations
LMARGN = $52
RMARGN = $53
ROWCRS = $54
COLCRS = $55
SAVMSC = $58

; extern void stringConcat(UInt8 *dst, const UInt8 *src);
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


; extern void stringCopy(UInt8 *dst, const UInt8 *src);
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


; extern UInt8 stringLength(UInt8 *s);
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


; extern UInt8 toAtascii(UInt8 c);
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


; extern void printLine(UInt8 *s);
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


; void printStringAtXY(const UInt8 *s, UInt8 x, UInt8 y);
.export _printStringAtXY
.proc _printStringAtXY
	sta ROWCRS   		; parameter 'y'

	jsr popa  			; parameter 'x'
	sta COLCRS 
	sta LMARGN
	
	jsr popptr1 		; parameter 's'
	lda ptr1
	ldx ptr1+1

	jsr _printLine
	rts
.endproc

