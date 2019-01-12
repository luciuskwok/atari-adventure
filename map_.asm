; map_.asm

.include "atari_memmap.asm"

.import _currentRunLenMap
.import _currentTileMap
.import _playerLocation
.import _mapSize
.import _mapFrame
.import _dliSpriteData


.code 


;  void drawCurrentMap(void);
.export _drawCurrentMap
.proc _drawCurrentMap
	; Draws the map in _currentRunLenMap with player at position in 
	; _mapCurrentLocation, using the layout in _mapFrame.
	; Also uses _mapSize.
	; Calls _drawMapRow, which uses sreg, ptr1, ptr2.
	; Calls _decodeRunLenRange, which uses and modifies sreg, ptr1.
	.importzp sp
	.importzp ptr1, ptr3
	.importzp tmp1, tmp2 
	.import addysp, subysp, pushax
	.import _setSavadrToGraphicsCursor 	; uses sreg, ptr1

	screenWidth = 24

	lda _currentRunLenMap+1 		; check MSB for NULL pointer
	bne passed_integrity_check
		rts 
	passed_integrity_check:

	buffer = sp 
		ldy #screenWidth 			; reserve 24 bytes for buffer
		jsr subysp

	outOfBoundsChar = OLDCHR 
		lda _currentTileMap 		; Temporarily use ptr3 for _currentTileMap 
		sta ptr3 					; and get the char to be used for out of
		lda _currentTileMap+1		; bounds tiles. 
		sta ptr3+1
		ldy #0
		lda (ptr3),Y
		sta outOfBoundsChar

	runLenPtr = ptr3 
		lda _currentRunLenMap
		sta runLenPtr 
		lda _currentRunLenMap+1
		sta runLenPtr+1

	frameHeight = _mapFrame+3
	mapRow = OLDROW
	topMargin = tmp1
		lda frameHeight 			; The mapFrame defines the size and location of
		lsr  						; the window into the tiles being shown.
		sec 						; topMargin = mapFrame.height/2 - playerLocation.y
		sbc _playerLocation+1
		bcc top_in_bounds  			; if result is positive: top is out of bounds
		top_out_of_bounds:		
			; In this case, the top edge of the frame is at or above the top edge of the map data, so there may be a top margin filled with the "out of bounds" rows.
			sta topMargin
			lda #0
			sta mapRow 
			jmp topMargin_done
		top_in_bounds:
			; In this case, the top edge of the frame is below the top edge of the map data. 
			eor #$FF 				; Convert negative to positive by flipping 
			tax 					; bits and adding 1.
			inx 
			stx mapRow 
			lda #0
			sta topMargin 
			loop_skip_rows:
				jsr inc_runLenPtr
				dex
				bne loop_skip_rows 
		topMargin_done:

	frameWidth = _mapFrame+2
	mapCol = OLDCOL
	leftMargin = LMARGN 
		lda frameWidth 				; leftMargin = mapFrame.width/2 - playerLocation.x
		lsr 
		sec 
		sbc _playerLocation 
		bcc left_in_bounds 
		left_out_of_bounds:
			; In this case, the left edge of the window shows tiles beyond the left edge of the map data. So the left margin is increased to indicate that how many "out of bounds" tiles to show.  
			sta leftMargin 			; leftMargin as above
			lda #0
			sta mapCol  			; mapCol = 0
			jmp leftMargin_done
		left_in_bounds:
			eor #$FF 				; Convert negative to positive by flipping 
			clc 					; bits and adding 1.
			adc #1 
			sta mapCol  			; mapCol = playerLocation.x - mapFrame.width/2
			lda #0
			sta leftMargin 			; leftMargin = 0
		leftMargin_done:

	rightMargin = RMARGN 
		; The right margin is where the map tile drawing ends, when screenCol is equal or greater than the right margin, draw the "out of bounds" tile.
		lda _mapSize 				; mapSize.width
		clc 
		adc leftMargin 
		sec 
		sbc mapCol 
		sta rightMargin 

	decodeLength = DELTAC
		;  Calculate the number of map tiles to decode on each line. This is the width of the window minus any left margin, which is the "out of bounds" area.
		lda frameWidth
		sec 
		sbc leftMargin 
		sta decodeLength 
		; For narrow maps, make sure the end doesn't extend beyond the right edge of the map data. 
		lda _mapSize 				; mapSize.width
		sec 
		sbc mapCol 
		cmp decodeLength  			; if mapSize.width - mapCol < decodeLength
		bcs decodeLength_done
			sta decodeLength 		; then: decodeLength = mapSize.width - mapCol
		decodeLength_done:

	screenAddr = SAVADR 
		lda _mapFrame
		sta COLCRS 
		lda _mapFrame+1
		sta ROWCRS 
		jsr _setSavadrToGraphicsCursor

	screenRow = ROWCRS 
		ldx #0
		stx screenRow				; reset screenRow to 0

	loop_row:
		lda #0
		sta _dliSpriteData,X 		; Clear the sprite overlay for this row

		lda screenRow  				; if screenRow < topMargin
		cmp topMargin 				; then: row is out of bounds
		bcc row_out_of_bounds

		lda mapRow 					; if mapRow >= mapHeight
		cmp _mapSize+1	 			; then: row is out of bounds
		bcs row_out_of_bounds

		row_in_bounds:
			lda buffer 
			ldx buffer+1
			jsr pushax 
			lda runLenPtr 
			ldx runLenPtr+1
			jsr _decodeRunLenRange 	; decodeRunLenRange(buffer, runLenPtr)

			jsr inc_runLenPtr 		; next row

			lda buffer 
			ldx buffer+1
			jsr _drawMapRow

			inc mapRow
			jmp next_row

		row_out_of_bounds:
			lda outOfBoundsChar 
			jsr _fillMapRow 

	next_row:
		lda screenAddr 
		clc 
		adc #screenWidth 
		sta screenAddr
		bcc @skip_msb 
			inc screenAddr+1
		@skip_msb: 

		ldx screenRow 
		inx 
		stx screenRow
		cpx frameHeight	 			; if screenRow < mapFrame.height
		bcc loop_row				; then: loop again

	return:
		ldy #screenWidth			; free buffer
		jsr addysp 
		rts 

	inc_runLenPtr:
		ldy #0
		lda (runLenPtr),Y 	; Jump to first visible row in map data
		clc 
		adc runLenPtr 
		sta runLenPtr
		bcc @skip_msb
			inc runLenPtr+1
		@skip_msb:
		rts
.endproc 


.proc _fillMapRow 
	; Fills tiles starting at SAVADR with A
	frameWidth = _mapFrame+2
	ldy #0 
	loop: 
		sta (SAVADR),Y
		iny 
		cpy frameWidth
		bne loop
	rts
.endproc


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
	.import _setTileSprite

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
	;   of output and writing out up to DELTAC bytes length.
	; * The format is: 4 bits of the tile value, then 4 bits of the repeat count.
	;   If those last 4 bits == $0F, add the value of the next 8 bits. 
	;   The output is tile value written out repeat+1 times. 
	; * Parameters:
	;   OLDCOL: number of bytes to skip before starting output.
	;   DELTAC: length of output requested
	.import 	popsreg, popa
	.importzp 	sreg, ptr1

.code
	src = ptr1 			; src is updated to point to next input byte
	sta src
	stx src+1

	jsr inc_src_byte 	; discard length byte

	dst = sreg  		; dst is updated to point to next output byte
	jsr popsreg

	skipRemaining = COLCRS 
	lda OLDCOL
	sta skipRemaining

	tile = TMPCHR 		; store tile value in OS location

	dstEnd = NEWCOL 	; NEWCOL = OLDCOL + DELTAC
	clc
	lda DELTAC
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
