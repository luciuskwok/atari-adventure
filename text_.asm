; text_.asm

.import 	addysp, popa, popptr1, popsreg, pushax, subysp
.import 	mulax10, udiv16
.import 	_zeroOutYAtPtr1, _multiplyAXtoPtr1
.import 	_charaAtIndex

.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

.import 	_partyMoney, _partyPotions, _partyFangs

.include "atari_memmap.asm"


; Constants
	ROW_BYTES = 40
	SPACE     = $20
	NEWLINE   = $9B ; newlines converted to ATASCII are $9B


; void printCharaAtIndex(UInt8 index);
.export _printCharaAtIndex
.proc _printCharaAtIndex 	; uses sreg, ptr1, ptr2, ptr3, ptr4
	.rodata 
		lvString:
		.byte "Lv "
		.byte 0

	.code 

	value = ptr1 
	string = ptr2 
	charaPtr = ptr3

	pha  

	jsr _charaAtIndex  		; get chara ptr
	sta charaPtr 
	stx charaPtr+1

	pla  					; x = index * 10
	ldx #0
	jsr mulax10 

	clc  					; x += 1
	adc #1
	sta COLCRS 
	sta LMARGN

	reserve_string:
		ldy #10
		jsr subysp
		lda sp 
		sta string
		lda sp+1
		sta string+1

	print_name:
		ldy #1
		lda (charaPtr),Y 
		tax 
		dey 
		lda (charaPtr),Y
		jsr _printLine 

	print_lv:
		
		jsr _stringCopy





	rts 
.endproc


; void printPartyStats(void);
.export _printPartyStats
.proc _printPartyStats
	ldy #32				; reserve 32 bytes on stack for string buffer
	jsr subysp 

	value = ptr1
	string = ptr2
	lda sp
	sta string 
	lda sp+1
	sta string+1

	; unit16StringInternal uses ptr1, ptr2, ptr4, sreg, tmp1

	party_money:
		lda #'$'
		jsr _appendCharToPtr2 		; start off with money symbol

		lda _partyMoney				; append money string
		sta value 
		lda _partyMoney+1
		sta value+1
		jsr _uint16StringInternal

		clc 
		adc string 					; move string pointer to end
		sta string
		bcc @skip_msb
		inc string+1

	@skip_msb:
		lda #' '
		jsr _appendCharToPtr2
		jsr _appendCharToPtr2

	party_potions:
		lda _partyPotions 			; append potions string
		sta value
		lda #0
		sta value+1
		jsr _uint16StringInternal

		clc 
		adc string 					; move string pointer to end
		sta string
		bcc @skip_msb
		inc string+1

	@skip_msb:
		lda #$11
		jsr _appendCharToPtr2
		lda #' '
		jsr _appendCharToPtr2
		jsr _appendCharToPtr2

	party_fangs:
		lda _partyFangs 			; append fangs string
		sta value
		lda #0
		sta value+1
		jsr _uint16StringInternal

		clc 
		adc string 					; move string pointer to end
		sta string
		bcc @skip_msb
		inc string+1

	@skip_msb:
		lda #$12
		jsr _appendCharToPtr2

	append_terminator:
		lda #0
		jsr _appendCharToPtr2

	center_text:
		lda sp  					; get string length
		sta ptr1 					; pass string in ptr1
		lda sp+1
		sta ptr1+1
		jsr _stringLengthInternal

		lsr a  						; colcrs = 20 - length / 2
		eor #$FF
		sec 
		adc #20 
		sta COLCRS 

		lda #5
		sta ROWCRS 

		lda sp
		ldx sp+1
		jsr _printLine

	ldy #32				; free 32 bytes on stack
	jsr addysp 

	rts
.endproc


; void drawTextBox(const UInt8 *s);
.export _drawTextBox
.proc _drawTextBox
	; COLCRS: starting & newline X-position.
	; ROWCRS: starting Y-position.
	; LMARGN: X-position after wrapping line
	; RMARGN: X-position of right margin
	; ROWINC: line spacing

	string = ptr2
	sta string 
	stx string+1

	screenRow = SAVADR  	; screenRow points at start of current row
	lda ROWCRS
	ldx #ROW_BYTES
	jsr _multiplyAXtoPtr1 	; ptr1 = rowcrs * row_bytes

	clc 					; screenRow = TXTMSC + ptr1
	lda TXTMSC
	adc ptr1
	sta screenRow
	lda TXTMSC+1
	adc ptr1+1
	sta screenRow+1

	newlineMargin = tmp1 
	lda COLCRS
	sta newlineMargin

	length = tmp2

	nonEmptyLine = tmp3
	lda #0 					; clear nonEmptyLine
	sta nonEmptyLine

	loop: 
		ldy #0
		lda (string),Y
		bne check_whitespace  		; return upon reaching null terminator
		rts 
		check_whitespace:
			cmp #SPACE
			beq print_space 		; print space between words
			cmp #NEWLINE
			beq print_newline 		; jump to next line
		check_wrap:
			jsr _wordLengthAtPtr2 	; check if word will fit
			sta length 
			sec
			lda RMARGN
			sbc length 
			cmp COLCRS 				; if rmargn-length => colcrs: print word
			bcc check_empty_line
		print_word:
			lda #1 					; set nonEmptyLine
			sta nonEmptyLine
			jsr _printStringAtPtr2
			jsr _ptr2AddTmp2		; string += length
			jmp loop 				; next loop
		check_empty_line:
			lda nonEmptyLine 		; if line is empty: print truncated line 
			bne wrap_line
		truncate_line:
			lda RMARGN  			; length = rmargn - colcrs
			sec 
			sbc COLCRS 
			sta length  			
			jsr _printStringAtPtr2
			jsr _ptr2AddTmp2		; fall through to wrap_line
		wrap_line:
			lda LMARGN 				; move cursor to left margin
			sta COLCRS
			jmp next_line
		print_space:
			lda COLCRS  			; if colcrs >= rmargn: wrap line instead
			cmp RMARGN
			bcs wrap_line 
			lda #1 					; print 1 space 
			sta length 
			lda nonEmptyLine 		; if line is empty, skip printing a space
			beq skip_extra_space 
			jsr _printStringAtPtr2
		skip_extra_space:
			jsr _ptr2AddTmp2		; string += length
			jmp loop
		print_newline:
			lda #1 					; skip 1 newline char 
			sta length 
			jsr _ptr2AddTmp2
			lda newlineMargin
			sta COLCRS 				; fall through to next_line
		next_line:
			lda #0 					; clear nonEmptyLine
			sta nonEmptyLine
			ldx ROWINC
		next_row:
			lda ROWCRS   			; move cursor to next row
			clc 
			adc #1 
			sta ROWCRS 
			cmp BOTSCR 				; if rowcrs >= botscr: return
			bcc next_screen_row 	; at or beyond bottom of screen
			rts
		next_screen_row:
			clc 					; move screenRow to next row
			lda screenRow
			adc #ROW_BYTES
			sta screenRow
			lda screenRow+1
			adc #0
			sta screenRow+1

			dex  					; repeat for ROWINC rows
			bne next_row

			jmp loop 				; next loop
.endproc


.proc _ptr2AddTmp2 
	clc 					; ptr2 += tmp2
	lda ptr2 
	adc tmp2 
	sta ptr2
	lda ptr2+1
	adc #0
	sta ptr2+1
	rts
.endproc 


.proc _printStringAtPtr2 
	; uses ptr1
	; SAVADR = start of screen row
	; COLCRS = x-position of cursor
	; ptr2 = string
	; tmp2 = length

	clc  				; ptr1 = savadr + colcrs
	lda SAVADR
	adc COLCRS
	sta ptr1 
	lda SAVADR+1
	adc #0
	sta ptr1+1

	ldy #0
	jmp while
	loop:
		lda (ptr2),Y 	; copy ptr2 to ptr1
		jsr _toAtascii
		sta (ptr1),Y
		iny 
	while:
		cpy tmp2 
		bne loop

	lda COLCRS 			; colcrs += length
	clc
	adc tmp2
	sta COLCRS 

	rts 
.endproc 


.proc _wordLengthAtPtr2
	; ptr2: string
	; returns length up to but not including space or newline char

	ldy #0
	loop:
		lda (ptr2),Y
		beq return 		; null termintor
		cmp #SPACE
		beq return 		; space
		cmp #NEWLINE
		beq return  	; newline
		iny  	
		beq return 		; max length
		jmp loop
	return:
		tya 
		rts 
.endproc


.proc _appendCharToPtr2
	; appends char in A to string at ptr2 and increments ptr2.
	; does not append null terminator
	ldy #0
	sta (ptr2),Y
	inc_ptr2:
		inc ptr2 
		bne return 
		inc ptr2+1
	return:
		rts 
.endproc 


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

	jsr _cursorAddressToPtr1

	index = tmp1
	lda #height
	sta index
	loop: 
		ldy #width 
		jsr _zeroOutYAtPtr1

		clc 				; ptr1 += row_bytes
		lda ptr1
		adc #ROW_BYTES 
		sta ptr1
		bcc next_loop
		inc ptr1+1
	next_loop:
		dec index
		bne loop
	rts
.endproc


; void stringConcat(UInt8 *dst, const UInt8 *src);
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

	add_dst:
		clc
		adc DST
		sta DST
		bcc skip_msb
		inc DST+1
	skip_msb:

	jsr _stringCopyInternal

	rts
.endproc

; void stringCopy(UInt8 *dst, const UInt8 *src);
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

; UInt8 stringLength(UInt8 *s);
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

; UInt8 toAtascii(UInt8 c);
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



.proc _cursorAddressToPtr1 
	; Returns screen memory address at text cursor position in ptr1
	; Calls _multiplyAXtoPtr1 (uses sreg)

	lda ROWCRS			; ptr1 = y * row_bytes
	ldx #ROW_BYTES
	jsr _multiplyAXtoPtr1

	clc 				; ptr1 += TXTMSC
	lda ptr1
	adc TXTMSC 
	sta ptr1
	lda ptr1+1
	adc TXTMSC+1
	sta ptr1+1

	clc 				; ptr1 += colcrs
	lda ptr1
	adc COLCRS 
	sta ptr1
	lda ptr1+1
	adc #0
	sta ptr1+1

	rts 
.endproc


; void printLine(UInt8 *s);
.export _printLine
.proc _printLine
	; uses ptr1, ptr2, AY
	cpx #0
	beq next_line		; if s == NULL: jump to next line

	sta ptr2 			; ptr2 = s
	stx ptr2+1

	jsr _cursorAddressToPtr1

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


; UInt8 uint8toString(UInt8 *string, UInt8 value);
.export _uint8toString
.proc _uint8toString 	; uses sreg, ptr1, ptr2, ptr4, tmp1
	sta ptr1 			; value
	lda #0
	sta ptr1+1

	jsr popsreg 		; string
	lda sreg 			; copy string to ptr2
	sta ptr2
	lda sreg+1
	sta ptr2+1

	jsr _uint16StringInternal
	rts 
.endproc


; UInt8 uint16toString(UInt8 *string, UInt16 value);
.export _uint16toString
.proc _uint16toString	; uses sreg, ptr1, ptr2, ptr4, tmp1
	; returns length of string
	sta ptr1 			; value
	stx ptr1+1

	jsr popsreg 		; string
	lda sreg 			; copy string to ptr2
	sta ptr2
	lda sreg+1
	sta ptr2+1

	jsr _uint16StringInternal
	rts 
.endproc 


.export _uint16StringInternal
.proc _uint16StringInternal	; uses sreg, ptr1, ptr2, ptr4, tmp1
	; on entry: 
	; ptr1 = value
	; ptr2 = output string
	; returns: length of string
	; udiv16 uses only sreg, ptr1, ptr4, AXY
	value = ptr1
	string = ptr2
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

		lda value 		; if value != 0: next loop
		bne loop_push
		lda value+1
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

	tya 				; return length
	ldx #0
	rts
.endproc


; void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y);
.export _debugPrint
.proc _debugPrint
	sta ROWCRS  		; parameter 'y'

	jsr popa  			; parameter 'x'
	sta COLCRS

	jsr popptr1 		; parameter 'value'
	lda ptr1
	pha  				; store 'value' on stack
	lda ptr1+1
	pha 

	jsr popptr1 		; parameter 's'
	lda ptr1 			; ptr2 is source for copy
	sta ptr2 
	lda ptr1+1
	sta ptr2+1 

	ldy #32				; reserve 32 bytes on stack for string buffer
	jsr subysp 

	lda sp
	sta ptr1 
	lda sp+1
	sta ptr1+1

	jsr _stringCopyInternal ; copy string from ptr2 to ptr1

	jsr _stringLengthInternal 
	clc 
	adc ptr1 			; put pointer to end of string in AX
	ldx ptr1+1
	jsr pushax 			; push string pointer on parameter stack

	pla 				; pull 'value' off stack
	tax 
	pla 
	jsr _uint16toString

	lda sp		 		; print string buffer
	ldx sp+1
	jsr _printLine 

	ldy #32				; free 32 bytes on stack
	jsr addysp
	
	rts 
.endproc
