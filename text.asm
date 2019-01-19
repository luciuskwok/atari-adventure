; text_.asm

.import 	addysp, popa, popptr1, popsreg, pushax, subysp
.import 	_zeroOutYAtPtr1
.import 	_addAToSavadr
.import 	_sizeBarChart, _drawBarChart


.include "atari_memmap.asm"


; Constants
	ROW_BYTES = 40
	SPACE     = $20
	TAB       = $7F
	NEWLINE   = $9B ; newlines converted to ATASCII are $9B


; Basic Printing

; void printString(const UInt8 *s);
.export _printString
.proc _printString 
	.importzp 	ptr2 	; uses ptr1, ptr2

	cmp #0 
	bne @skip_msb
		cpx #0
		beq return 		; if s == NULL: return
	@skip_msb:

	string = ptr2 
		sta string 
		stx string+1

	jsr _setSavadrToTextCursor ; uses ptr1

	ldy #0
	loop:
		lda(string),Y
		beq inc_colcrs		; if string[Y] == 0: break
		jsr _toAtascii
		ora BITMSK
		sta(SAVADR),Y		; screen[Y] = string[Y]
		iny
		bne loop
	inc_colcrs:
		tya 
		clc 
		adc COLCRS
		sta COLCRS
	return:
		rts
.endproc 


; void moveToNextLine(void);
.export _moveToNextLine 
.proc _moveToNextLine 
	lda LMARGN 
	sta COLCRS 
	inc ROWCRS
	rts 	
.endproc 


; void printLine(UInt8 *s);
.export _printLine
.proc _printLine
	jsr _printString 
	jsr _moveToNextLine
	rts
.endproc


; void printStringAtXY(const UInt8 *s, UInt8 x, UInt8 y);
.export _printStringAtXY
.proc _printStringAtXY
	.importzp 	ptr1

	sta ROWCRS   		; parameter 'y'

	jsr popa  			; parameter 'x'
	sta COLCRS 
	sta LMARGN

	lda #0
	sta BITMSK 			; set BITMASK to 0 
	
	jsr popptr1 		; parameter 's'
	lda ptr1
	ldx ptr1+1

	jsr _printLine
	rts
.endproc


; void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y);
.export _debugPrint
.proc _debugPrint
	.importzp 	sp, ptr1, ptr2

	sta ROWCRS  		; parameter 'y'

	jsr popa  			; parameter 'x'
	sta COLCRS

	jsr popptr1 		; parameter 'value'
	lda ptr1
	pha  				; store 'value' on stack
	lda ptr1+1
	pha 

	lda #0
	sta BITMSK 			; set BITMASK to 0 

	jsr popptr1 		; print parameter 's'
	lda ptr1 
	ldx ptr1+1
	jsr _printString 

	ldy #6				; reserve 6 bytes on stack for string buffer
	jsr subysp 

	lda sp 				; ptr2 = output string for _uint16StringInternal
	sta ptr2 
	lda sp+1
	sta ptr2+1

	pla 				; pull 'value' off stack
	sta ptr1+1  			; ptr1 = value for _uint16StringInternal
	pla 
	sta ptr1 
	jsr _uint16StringInternal

	lda sp		 		; print string buffer
	ldx sp+1
	jsr _printLine 

	ldy #6				; free 6 bytes on stack
	jsr addysp
	
	rts 
.endproc


; Stat Printing

; void printAllCharaStats(UInt8 row);
.export _printAllCharaStats
.proc _printAllCharaStats
	.import _partySize

	sta OLDROW 	; store row in OLDROW 

	lda #1  	; start at x-position = 1
	sta LMARGN

	ldx #0 		; X = index
	loop:
		lda OLDROW
		sta ROWCRS 		; reset ROWCRS = row

		lda LMARGN
		sta COLCRS 

		txa
		pha  			; push index on stack

		ldx #0
		jsr _printCharaAtIndex

		pla  			; pull index from stack
		tax 

		lda LMARGN  	; move to next column
		clc 
		adc #10 
		sta LMARGN

		inx
	while:
		cpx _partySize
		bcc loop
	rts
.endproc 


; void eraseCharaBoxAtIndex(UInt8 index);
.export _eraseCharaBoxAtIndex
.proc _eraseCharaBoxAtIndex
	; _setSavadrToTextCursor uses AXY, sreg, ptr1

	width = 9
	height = 4

	jsr _setSavadrToTextCursor

	ldx #height
	loop_row: 
		lda #0
		ldy #width 
		loop_col:
			dey
			sta (SAVADR),Y
			bne loop_col
		lda #ROW_BYTES 				; SAVADR += row_bytes
		jsr _addAToSavadr
	next_row:
		dex
		bne loop_row
	rts
.endproc


; void printCharaAtIndex(UInt8 index);
.export _printCharaAtIndex
.proc _printCharaAtIndex 	; uses sp, sreg, ptr1, ptr2, ptr3, ptr4, tmp1
	.importzp sp, ptr1, ptr2, ptr3, tmp1
	.import _partyChara

	.rodata 
		lvString: .byte "Lv."
		hpString: .byte "/"
	.code

	; Chara struct offsets
	charaLevel = 2 
	charaHp = 3 
	charaMaxHp = 4

	charaPtr = ptr3
		asl a  					; charaPtr = partyChara + index * 16
		asl a 
		asl a 
		asl a 
		clc 
		adc #<_partyChara 
		sta charaPtr 
		lda #>_partyChara 
		adc #0
		sta charaPtr+1

	clear_bitmsk:
		lda #0
		sta BITMSK 

	print_name:
		ldy #1 
		lda (charaPtr),Y
		tax 
		dey 
		lda (charaPtr),Y 
		jsr _printLine  	; uses sreg, ptr1, AXY

	reserve_string:
		ldy #6
		jsr subysp 

	print_lv:
		lda #<lvString		; string
		ldx #>lvString 
		ldy #3  			; length
		jsr print_string_axy 

		ldy #charaLevel 
		lda (charaPtr),Y 
		jsr print_value

		jsr _moveToNextLine

	print_hp:
		ldy #charaHp 
		lda (charaPtr),Y 
		jsr print_value

		lda #<hpString		; string
		ldx #>hpString 
		ldy #1  			; length
		jsr print_string_axy 

		ldy #charaMaxHp 
		lda (charaPtr),Y
		jsr print_value

		jsr _moveToNextLine

	draw_hp_bar:
		lda COLCRS  		; Temporarily set COLCRS to 0 so 
		pha 				; that SAVADR points to row start.
		lda #0
		sta COLCRS
		jsr _setSavadrToTextCursor
		pla 
		asl a 
		asl a   			; Multiply COLCRS by 4 to switch to
		sta COLCRS  		; raster pixels.

		ldy #charaMaxHp 
		lda (charaPtr),Y
		tax
		ldy #charaHp	 	
		lda (charaPtr),Y
		jsr _sizeBarChart  	; A=hp, X=maxHp
		jsr _drawBarChart

	free_string:
		ldy #6
		jsr addysp
	rts 

	print_string_axy:
		sta ptr1  			; AX: ptr1: string
		stx ptr1+1
		sty tmp1 			; Y: tmp1: length
		jsr _printStringWithLength 
		rts 

	print_value:
		sta ptr1 	; ptr1: value
		lda #0
		sta ptr1+1
		lda sp 		; ptr2: output string
		sta ptr2 
		lda sp+1
		sta ptr2+1 
		jsr _uint16StringInternal ; uses sreg, ptr1, ptr2, ptr4, tmp1

		tay 		; Y: length
		lda sp  	; AX: string
		ldx sp+1 
		jsr print_string_axy
		rts 
.endproc


; void printPartyStats(void);
.export _printPartyStats
.proc _printPartyStats
	.importzp 	sp, ptr1, ptr2
	.import 	_partySize, _partyMoney, _partyPotions, _partyFangs
	; unit16StringInternal uses ptr1, ptr2, ptr4, sreg, tmp1

	ldy #32				; reserve 32 bytes on stack for string buffer
	jsr subysp 

	potionChar = 14
	fangChar = 15 

	value = ptr1
	string = ptr2
		lda sp
		sta string 
		lda sp+1
		sta string+1

	clear_bitmsk:
		lda #0
		sta BITMSK 

	party_money:
		lda #'$'
		jsr append_char 		; start off with money symbol

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
		jsr append_char
		jsr append_char

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
		lda #potionChar
		jsr append_char
		lda #' '
		jsr append_char
		jsr append_char

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
		lda #fangChar
		jsr append_char

	append_terminator:
		lda #0
		jsr append_char

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

	append_char:
		; appends char in A to string at ptr2 and moves ptr2 to end.
		; does not append null terminator
		ldy #0
		sta (ptr2),Y
		inc ptr2 
		bne @skip_msb 
			inc ptr2+1
		@skip_msb:
		rts 
.endproc 


; Formatted Printing

; UInt8* drawTextBox(const UInt8 *s);
.export _drawTextBox
.proc _drawTextBox
	; COLCRS: starting & newline X-position.
	; ROWCRS: starting Y-position.
	; LMARGN: X-position after wrapping line
	; RMARGN: X-position of right margin
	; ROWINC: line spacing
	; Returns: pointer to end of printed text
	.importzp 	ptr1, tmp1, tmp3, tmp4
	.import 	_addAToPtr1

	string = ptr1
	sta string 
	stx string+1

	length = tmp1

	newlineMargin = tmp4
	lda COLCRS
	sta newlineMargin

	nonEmptyLine = tmp3
	lda #0 					; clear nonEmptyLine
	sta nonEmptyLine

	loop: 
		ldy #0
		lda (string),Y
		bne check_whitespace  		; return upon reaching null terminator
			lda string 
			ldx string+1 			; return pointer to end of text
			rts 
		check_whitespace:
			cmp #SPACE
			beq print_space 		; print space between words
			cmp #TAB
			beq print_tab
			cmp #NEWLINE
			beq print_newline 		; jump to next line
		check_wrap:
			jsr word_length		 	; check if word will fit
			sta length 
			sec
			lda RMARGN
			sbc length 
			cmp COLCRS 				; if rmargn-length => colcrs: print word
			bcc check_empty_line
		print_word:
			lda #1 					; set nonEmptyLine
			sta nonEmptyLine
			jsr _printStringWithLength
			jmp loop 				; next loop

		check_empty_line:
			lda nonEmptyLine 		; if line is empty: print truncated line 
			bne wrap_line
		truncate_line:
			lda RMARGN  			; length = rmargn - colcrs
			sec 
			sbc COLCRS 
			sta length  			
			jsr _printStringWithLength
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
			beq skip_char 
			jsr _printStringWithLength
			jmp loop

		print_tab: 
			; print tab by skipping 4 spaces
			lda COLCRS 
			clc 
			adc #4
			sta COLCRS 

		skip_char:
			lda #1
			jsr _addAToPtr1			; string += length
			jmp loop
		print_newline:
			lda #1 					; skip 1 newline char 
			sta length 
			jsr _addAToPtr1
			lda newlineMargin
			sta COLCRS 				; fall through to next_line
		next_line:
			lda #0 					; clear nonEmptyLine
			sta nonEmptyLine
			ldx ROWINC
		next_row:
			ldy ROWCRS   			; move cursor to next row
			iny 
			sty ROWCRS 
			cpy BOTSCR 				; if rowcrs >= botscr: return
			bcc next_screen_row 	; at or beyond bottom of screen
		return_at_bottom:
			lda string 
			ldx string+1 			; return pointer to end of text
			rts
		next_screen_row:
			lda #ROW_BYTES			; move SAVADR to next row
			jsr _addAToSavadr
			dex  					; repeat for ROWINC rows
			bne next_row

			jmp loop 				; next loop

	word_length:
		; returns length up to but not including space or newline char	
		ldy #0
		@loop:
			lda (string),Y
			beq @return 		; null termintor
			cmp #SPACE
			beq @return 		; space
			cmp #NEWLINE
			beq @return  	; newline
			iny  	
			beq @return 		; max length
			jmp @loop
		@return:
			tya 
			rts 
.endproc


; Internal Printing Routines

.proc _printStringWithLength 
	; uses ptr1, SAVADR
	.importzp 	ptr1, tmp1
	.import 	_addAToPtr1

	string = ptr1 
	length = tmp1 

	lda ptr1  			; save ptr1 on stack
	pha 
	lda ptr1+1
	pha 

	jsr _setSavadrToTextCursor 	; uses AXY, sreg, ptr1

	pla 
	sta ptr1+1
	pla 
	sta ptr1

	ldy #0
	jmp while
	loop:
		lda (string),Y 	; copy string to screen
		jsr _toAtascii
		sta (SAVADR),Y
		iny 
	while:
		cpy length 
		bne loop

	lda COLCRS 			; COLCRS += length
	clc
	adc length
	sta COLCRS 

	lda length 			; string += length
	jsr _addAToPtr1

	rts 
.endproc 


.export _setSavadrToTextCursor
.proc _setSavadrToTextCursor 
	; Stores cursor address in SAVADR
	; Calls _mulax40 (uses  ptr1)
	.importzp 	ptr1
	.import _mulax40

	lda ROWCRS			; ptr1 = y * row_bytes
	ldx #0
	jsr _mulax40

	clc 				; SAVADR = ptr1 + TXTMSC
	adc TXTMSC 
	sta SAVADR
	txa
	adc TXTMSC+1
	sta SAVADR+1

	lda COLCRS
	jsr _addAToSavadr

	rts 
.endproc


; String Utilities

; void stringConcat(UInt8 *dst, const UInt8 *src);
.export _stringConcat 		
.proc _stringConcat
	; ptr1 is _stringLength parameter.
	.importzp 	ptr1, ptr2
	.import 	_addAToPtr1

	SRC = ptr2
	sta SRC
	stx SRC+1
	DST = ptr1
	jsr popptr1

	jsr _stringLengthInternal ; DST += stringLength(DST)

	add_dst:
		jsr _addAToPtr1
	skip_msb:

	jsr _stringCopyInternal

	rts
.endproc

; void stringCopy(UInt8 *dst, const UInt8 *src);
.export _stringCopy 	
.proc _stringCopy
	.importzp 	ptr1, ptr2

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
	.importzp 	ptr1, ptr2

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
	.importzp 	ptr1

	STR = ptr1
	sta STR
	stx STR+1
	jsr _stringLengthInternal
	rts
.endproc

.proc _stringLengthInternal
	; On input, ptr1 points to string
	.importzp 	ptr1

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


; UInt8 uint8toString(UInt8 *string, UInt8 value);
.export _uint8toString
.proc _uint8toString
	.importzp 	sreg, ptr1, ptr2, ptr4, tmp1

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
.proc _uint16toString
	.importzp 	sreg, ptr1, ptr2, ptr4, tmp1
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
.proc _uint16StringInternal
	; on entry: 
	; ptr1 = value
	; ptr2 = output string
	; returns: length of string
	; udiv16 uses only sreg, ptr1, ptr4, AXY
	.importzp sreg, ptr1, ptr2, ptr4, tmp1
	.import udiv16

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


