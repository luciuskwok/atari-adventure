; map_.asm

.include "atari_memmap.asm"

.import _mapFrame
.import _currentTileMap

.import _setTileSprite

.code 


; void fillMapRow(UInt8 c);
.export _fillMapRow
.proc _fillMapRow 
	; Fills tiles starting at SAVADR to NEWCOL width with A
	ldy #0 
	loop: 
		sta (SAVADR),Y
		iny 
		cpy NEWCOL
		bne loop
	rts
.endproc

; void drawMapRow(UInt8 *buffer);
.export _drawMapRow
.proc _drawMapRow
	; Draws map tiles in buffer to screen. 
	; Parameters:
	; AX: input buffer pointer
	; SAVADR: pointer to screen row memory, offset to starting column
	; COLCRS: cursor column in screen coordinates 
	; ROWCRS: cursor row in screen coordinates 
	; LMARGN: number of tiles beyond left edge map data to show
	; RMARGN: width of map window 
	.importzp sreg, ptr2 ; _setTileSprite uses ptr1

	buffer = ptr2 			; Change this to use sp?
		sta buffer 
		stx buffer+1 

	tileMap = sreg 
		lda _currentTileMap
		sta tileMap 
		lda _currentTileMap+1
		sta tileMap+1

	windowWidth = _mapFrame+2
	tCastle = 19

	lda #0
	sta COLCRS 

	loop:
		cmp LMARGN  			; if screenCol < leftMargin
		bcc out_of_bounds
		cmp RMARGN 				; if screenCol >= rightMargin
		bcs out_of_bounds 
		in_bounds:
			sec  				; get tile value from buffer
			sbc LMARGN 
			tay 
			lda (buffer),Y
			jmp draw_char
		out_of_bounds:
			lda #0 				; use default value
		draw_char:
			tay 				; convert tile value to character value
			lda (tileMap),Y
			ldy COLCRS 
			sta (SAVADR),Y 		; put char on screen
		add_sprite:
			and #$3F
			sec 
			sbc #tCastle
			bcc next_loop
			jsr _setTileSprite
	next_loop:
		lda COLCRS 
		clc 
		adc #1
		sta COLCRS 				
		cmp windowWidth			; if screenCol < windowWidth: loop again
		bcc loop

	rts 
.endproc


; void decodeRunLenRange(UInt8 *outData, const UInt8 *runLenData);
.export _decodeRunLenRange 
.proc _decodeRunLenRange
	; * Decodes custom run-length encoded format data, skipping OLDCOL bytes
	;   of output and writing out up to DELTAR bytes length.
	; * The format is: 4 bits of the tile value, then 4 bits of the repeat count.
	;   If those last 4 bits == $0F, add the value of the next 8 bits. 
	;   The output is tile value written out repeat+1 times. 
	; * Parameters:
	;   OLDCOL: number of bytes to skip before starting output.
	;   DELTAR: length of output requested
	.import 	popptr1, popa
	.importzp 	ptr1, ptr2, ptr3
	.importzp 	tmp1

.code
	src = ptr3 			; src is updated to point to next input byte
	sta src
	stx src+1

	jsr inc_src_byte 	; discard length byte

	dst = ptr1  		; dst is updated to point to next output byte
	jsr popptr1

	skipRemaining = COLCRS 
	lda OLDCOL
	sta skipRemaining

	tile = TMPCHR 		; store tile value in OS location

	dstEnd = ptr2
	clc
	lda DELTAR
	adc dst 
	sta dstEnd
	lda #0
	adc dst+1
	sta dstEnd+1

	jmp next_input

	loop_input:
		ldy #0
		lda (src),Y
		jsr inc_src_byte 
		tax
		push_tile:
			and #$F0			; tile = top 4 bits of A
			lsr a 				; A = (A & 0xF0) >> 4 
			lsr a
			lsr a
			lsr a
			sta tile
		get_repeat:
			txa
			and #$0F 			; repeat = bottom 4 bits of A
			cmp #$0F
			bne check_skip		; if repeat == $0F: add next byte
		more_repeat:
			clc				
			adc (src),Y
			jsr inc_src_byte 
		check_skip:				
			cmp skipRemaining	; if repeat < skip: fast-fwd past this block
			bcs start_output
		ffwd_skip:
			eor #$FF			; subtract X from skipRemaining by using two's 
			clc 				; complement to negate X and adding to skipRemaining
			adc skipRemaining
			sta skipRemaining
			jmp next_input
		start_output:
			tax  				; put repeat on X
			inx
			jmp next_output
		loop_output:
			jsr output_byte
			dex 
		next_output:
			cpx #0
			beq next_input
			lda dst  			; if dst == dstEnd: return
			cmp dstEnd
			bne @skip_msb
				lda dst+1
				cmp dstEnd+1
				beq return 
			@skip_msb:
			jmp loop_output

	next_input:				; while (dst != dstEnd)
		lda dst 
		cmp dstEnd
		bne loop_input
		lda dst+1
		cmp dstEnd+1
		bne loop_input
	return:
		rts

	inc_src_byte:
		inc src 
		bne @skip_msb
			inc src+1
		@skip_msb:
		rts 

	output_byte:
		ldy skipRemaining 
		beq put_dst_byte
		dec_skipRemaining:
			dey
			sty skipRemaining
			rts 
		put_dst_byte:
			ldy #0
			lda tile
			sta (dst),Y
			inc dst 
			bne @skip_msb
				inc dst+1
			@skip_msb:
			rts 
.endproc
