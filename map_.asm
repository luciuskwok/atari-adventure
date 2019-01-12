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

	buffer = ptr2 
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
	; Parameters:
	; OLDCOL: number of bytes to skip before starting output.
	; DELTAR: length of output requested
	.import 	popptr1, popa
	.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

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

	OUT_LEN = DELTAR

	SKIP = COLCRS
	lda OLDCOL 
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
