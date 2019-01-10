; text_.asm

.import 	popa, popptr1, popsreg, pushax, udiv16
.import 	mulax10
.import 	_textWindow
.import 	_zeroOut8
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

; Zeropage locations
	LMARGN = $52
	RMARGN = $53
	ROWCRS = $54
	COLCRS = $55
	SAVMSC = $58

; Constants
	ROW_BYTES = 40


; void eraseCharaBoxAtIndex(UInt8 index);
.export _eraseCharaBoxAtIndex
.proc _eraseCharaBoxAtIndex
	width = 9
	height = 4

	ldx #0 				; colcrs = 1 + index * 10
	jsr mulax10
	clc 
	adc #1 
	sta COLCRS

	lda ROWCRS			; ptr1 = y * row_bytes
	ldx #ROW_BYTES
	jsr _multiplyAXtoPtr1

	clc 				; ptr1 += textWindow
	lda ptr1
	adc _textWindow 
	sta ptr1
	lda ptr1+1
	adc _textWindow+1
	sta ptr1+1

	clc 				; ptr1 += colcrs
	lda ptr1
	adc COLCRS 
	sta ptr1
	lda ptr1+1
	adc #0
	sta ptr1+1

	index = tmp1
	lda #height
	sta index
	loop: 
		lda ptr1 
		ldx ptr1+1
		jsr pushax 

		lda #width
		jsr _zeroOut8

		clc 				; ptr1 += row_bytes
		lda ptr1
		adc #ROW_BYTES 
		sta ptr1
		lda ptr1+1
		adc #0
		sta ptr1+1

		dec index
		bne loop
	rts
.endproc

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


; void printLine(UInt8 *s);
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


; void uint8toString(UInt8 *string, UInt8 value);
.export _uint8toString
.proc _uint8toString
	sta ptr1 			; value
	lda #0
	sta ptr1+1

	jsr popsreg 		; string
	string = ptr2
	lda sreg 			; copy string to ptr2
	sta string
	lda sreg+1
	sta string+1

	index = tmp1

	; compute length of output string
	lda ptr1
	cmp #100 
	bcs gte_100
	cmp #10
	bcs gte_10
	under_10: 
		ldy #1
		jmp set_terminator
	gte_10:
		ldy #2
		jmp set_terminator
	gte_100:
		ldy #3

	set_terminator:
	lda #0
	sta (string),Y
	sty index

	loop:
		lda #10
		sta ptr4
		lda #0
		sta ptr4+1

		jsr udiv16 		; divide ptr1 by ptr4, result in ptr1, remainder in sreg

		lda sreg
		clc 
		adc #$30

		ldy index 
		dey 
		sta (string),Y
		sty index

		cpy #0
		bne loop
	rts
.endproc


; void uint16toString(UInt8 *string, UInt16 value);
.export _uint16toString
.proc _uint16toString
	sta ptr1 			; value
	stx ptr1+1

	jsr popsreg 		; string
	string = ptr2
	lda sreg 			; copy string to ptr2
	sta string
	lda sreg+1
	sta string+1

	index = tmp1
	lda #0
	sta index

	loop_push:
		lda #10 		; reset ptr4 to 10 because udiv16 will modify it
		sta ptr4
		lda #0
		sta ptr4+1

		jsr udiv16 		; divide ptr1 / ptr4: ptr1=result, sreg=remainder

		lda sreg 		; get remainder
		clc 
		adc #$30 		; add ascii '0' char to remainder to get digits

		pha 			; push digit onto stack
		inc index 

		lda ptr1 		; if ptr1 != 0: next loop
		bne loop_push
		lda ptr1+1
		bne loop_push 

	ldy #0 
	loop_pull:
		pla  			; pull digits
		sta (string),Y	; and add them in the correct order
		iny 
		cpy index 
		bne loop_pull

	lda #0 				; terminate string with NULL
	sta (string),Y

	rts
.endproc

