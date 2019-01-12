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
	; Paramters in zeropage:
	; SAVADR: pointer to screen row memory, offset to starting column
	; COLCRS: cursor column in screen coordinates 
	; ROWCRS: cursor row in screen coordinates 
	; LMARGN: number of tiles beyond left edge map data to show
	; RMARGN: width of map window 
	.importzp ptr2, ptr3 

	buffer = ptr2 
		sta buffer 
		stx buffer+1 

	tileMap = ptr3 
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

