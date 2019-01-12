; sprites_.asm

.include "atari_memmap.asm"

; Constants 
	PM_LEFT_MARGIN = 48
	PM_TOP_MARGIN = 14 

.import _spriteArea
.import _addAToPtr1
.import _dliSpriteData


.rodata 
tileSpriteData:
	.byte $66, $99, $81, $42, $42, $81, $99, $66	; Castle  
	.byte $EE, $82, $82, $00, $82, $82, $EE, $00	; Town    
	.byte $00, $2C, $60, $06, $50, $44, $10, $00	; Village 
	.byte $00, $00, $00, $18, $18, $00, $00, $00	; Monument
	.byte $00, $00, $18, $24, $24, $24, $00, $00	; Cave    
	.byte $3E, $3E, $22, $22, $22, $22, $22, $00	; HouseDoor

.code 

; void setTileSprite(UInt8 spriteIndex)
.export _setTileSprite
.proc _setTileSprite
	; Add an overlay sprite for certain tiles. Mask sprites at edge of window, 
	; because of the half-width tiles at the left and right edges.
	; On entry: COLCRS & ROWCRS are set to the tile position.

	.importzp ptr1 	

	jsr _mulAx8		; multiply A by 8 to get sprite data offset
	tax 

	mask = BITMSK 
		lda COLCRS 
		beq mask_left
		cmp #20 
		beq mask_right
		no_mask:
			lda #$FF
			sta mask 
			jmp end_mask
		mask_left:
			lda #$0F
			sta mask
			jmp end_mask
		mask_right:
			lda #$F0
			sta mask
		end_mask:

	sprite = ptr1 
		player_3_offset = 384 + 128 * 4
		clc 
		lda _spriteArea
		adc #<player_3_offset
		sta sprite 
		lda _spriteArea+1
		adc #>player_3_offset
		sta sprite+1 

		lda ROWCRS 
		jsr _mulAx8
		clc 
		adc #PM_TOP_MARGIN
		jsr _addAToPtr1

	ldy #0
	copy_sprite_data:
		lda tileSpriteData,X
		and mask
		sta (sprite),Y 
		inx 
		iny 
		cpy #8
		bne copy_sprite_data
	set_dli:
		lda COLCRS 
		jsr _mulAx8
		clc 
		adc #(PM_LEFT_MARGIN - 4)
		ldx ROWCRS
		sta _dliSpriteData,X
	rts
.endproc

.proc _mulAx8
	asl a 
	asl a 
	asl a 
	rts
.endproc 

