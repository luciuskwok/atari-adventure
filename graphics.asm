; graphics_.asm

.import _multiplyAXtoPtr1

.include "atari_memmap.asm"

.data 
.export _dliSpriteData
_dliSpriteData:
	.res 10, $00

.code 

; Display List instructions
	DL_JVB    = $41
	DL_BLK1   = $00
	DL_BLK2   = $10
	DL_BLK3   = $20
	DL_BLK4   = $30
	DL_BLK5   = $40
	DL_BLK6   = $50
	DL_BLK7   = $60
	DL_BLK8   = $70
	DL_HSCROL = $10
	DL_VSCROL = $20
	DL_LMS    = $40
	DL_DLI    = $80
	DL_TEXT   = $02
	DL_TEXT_SPACER = $0F
	DL_TILE   = $07
	DL_RASTER = $0D

	LMS_GR    = $00 		; placeholder for SAVMSC
	LMS_TXT   = $01 		; placeholder for TXTMSC


; Constants
	ROW_BYTES = 40

; Screen modes
	ScreenModeMap    = 1
	ScreenModeDialog = 2
	ScreenModeBattle = 3
	ScreenModeInfo   = 4

; extern void setScreenMode(UInt8 mode);
.export _setScreenMode
.proc _setScreenMode
	.rodata
	packedMapDL: ; display list in PackBits format
		.byte   3-1,   DL_TILE|DL_DLI|DL_HSCROL|DL_LMS, LMS_GR, 0 ; tile, 9 rows
		.byte  -8+257, DL_TILE|DL_DLI|DL_HSCROL

		.byte  20-1,   DL_BLK6, DL_TEXT|DL_DLI|DL_LMS, LMS_TXT, 0 ; text, 3 rows
		.byte          DL_TEXT, DL_TEXT|DL_DLI

		.byte          DL_BLK1, DL_RASTER			 	; HP bar
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 3 	; add 3 rows to TXTMSC
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 3, DL_BLK1|DL_DLI

		.byte          DL_BLK7, DL_TEXT_SPACER, DL_TEXT, DL_TEXT_SPACER|DL_DLI
		.byte          DL_JVB
		.byte 128 ; terminator

	packedInfoDL: ; display list in PackBits format
		.byte   3-1,   DL_RASTER|DL_LMS, LMS_GR, 0 ; raster, 24 rows
		.byte -21+257, DL_RASTER 
		.byte   4-1,   DL_RASTER|DL_DLI, DL_RASTER|DL_DLI
		.byte          DL_TEXT|DL_DLI, DL_BLK1	; chara name
		.byte -11+257, DL_TEXT 					; chara stats
		.byte   4-1,   DL_TEXT|DL_DLI, DL_BLK8 
		.byte          DL_TEXT|DL_DLI, DL_BLK1 ; "Items"
		.byte  -3+257, DL_TEXT 					; items body
		.byte   2-1,   DL_TEXT|DL_DLI 			; last line DLI
		.byte          DL_JVB
		.byte 128 ; terminator

	packedDialogDL: ; display list in PackBits format
		.byte   3-1,   DL_RASTER|DL_LMS, LMS_GR, 0 ; raster, 72 rows
		.byte -71+257, DL_RASTER 
		.byte   3-1,   DL_TEXT|DL_LMS, LMS_TXT, 0 ; text, 7 rows
		.byte  -6+257, DL_TEXT 
		.byte   1-1,   DL_JVB
		.byte 128 ; terminator

	packedBattleDL: ; display list in PackBits format
		.byte   3-1,   DL_RASTER|DL_LMS, LMS_GR, 0 	; raster, 44 rows
		.byte -43+257, DL_RASTER|DL_DLI

		.byte  12-1,   DL_BLK1
		.byte          DL_RASTER				 	; enemy HP bar
		.byte          DL_RASTER|DL_LMS, LMS_GR, 44
		.byte          DL_RASTER|DL_LMS, LMS_GR, 44
		.byte          DL_BLK3 						; 9

		.byte          DL_TEXT|DL_LMS, LMS_TXT, 0 	; dialog box, 5 lines
		.byte  -3+257, DL_TEXT 
		.byte  18-1,   DL_TEXT|DL_DLI 
		.byte          DL_BLK6
		.byte          DL_TEXT|DL_DLI 				; chara name
		.byte          DL_TEXT						; chara level
		.byte          DL_TEXT|DL_DLI 				; 5

		.byte          DL_BLK1
		.byte          DL_RASTER				 	; player HP bars
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 8
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 8
		.byte          DL_BLK1|DL_DLI 				; 14

		.byte          DL_BLK4

		.byte          DL_RASTER|DL_LMS, LMS_GR, 45	; raster, 10 rows
		.byte  -9+257, DL_RASTER 
		.byte   3-1,   DL_BLK8, DL_BLK8|DL_DLI
		.byte          DL_JVB

		.byte 128 ; terminator

	.code 

	.import _mapViewDLI
	.import _battleViewDLI
	.import _infoViewDLI

	anticOptions = $2E 	; normal playfield, enable players & missiles, enable DMA
	enableDLI    = $C0  ; enable VBI + DLI

	pha		  			; push mode on stack
	
	lda #0 				; Turn off screen
	sta SDMCTL
	;sta DMLCTL
	lda #$40 			; disable DLI, leave VBI enabled
	sta NMIEN

	lda #7	 			; text window height
	sta BOTSCR

	lda #0				; graphics mode
	sta DINDEX 			; 0 for normal 40 bytes per row.

	lda #1
	jsr _delayTicks

	switch_mode:
		pla 			; pull mode off stack
		cmp #ScreenModeMap
			beq init_map
		cmp #ScreenModeInfo
			beq init_info
		cmp #ScreenModeDialog
			beq init_dialog
		cmp #ScreenModeBattle
			beq init_battle
		rts 				; default: screen off

	init_map:
		lda #<packedMapDL
		ldx #>packedMapDL
		jsr _unpackDisplayList

		lda #<_mapViewDLI
		sta VDSLST
		lda #>_mapViewDLI
		sta VDSLST+1

		lda #1				; graphics mode
		sta DINDEX 			; 1 for 24 bytes per row.

		jmp enable_screen

	init_info:
		lda #<packedInfoDL
		ldx #>packedInfoDL
		jsr _unpackDisplayList

		lda #18	 			; text window height
		sta BOTSCR

		lda #<_infoViewDLI
		sta VDSLST
		lda #>_infoViewDLI
		sta VDSLST+1

		jmp enable_screen

	init_dialog:
		lda #<packedDialogDL
		ldx #>packedDialogDL
		jsr _unpackDisplayList

		lda #0
		sta VDSLST
		sta VDSLST+1

		jmp enable_screen

	init_battle:
		lda #<packedBattleDL
		ldx #>packedBattleDL
		jsr _unpackDisplayList

		lda #9	 			; text window height
		sta BOTSCR

		lda #<_battleViewDLI
		sta VDSLST
		lda #>_battleViewDLI
		sta VDSLST+1

		jmp enable_screen

	enable_screen:
		lda VDSLST+1
		beq enable_dma 
	enable_dli:
		lda #enableDLI
		sta NMIEN
	enable_dma:
		lda #anticOptions
		sta SDMCTL
	rts
.endproc 


.proc _unpackDisplayList 
	; Reads PackBits data into display list area.
	; * On entry: AX = PackBits-compressed data
	; * uses ptr1, ptr2, tmp1
	; * On return: ptr1 points at end of written data
	.importzp ptr1, ptr2
	.import pushax
	
	src = ptr2 
	sta ptr2 
	stx ptr2+1

	dest = ptr1  		; start on byte 3 of display list area
	clc 
	lda SDLSTL
	adc #3  
	sta ptr1
	lda SDLSTL+1
	adc #0
	sta ptr1+1

	lda dest 
	ldx dest+1
	jsr pushax 
	lda src 
	ldx src+1
	jsr _unpackbits

	jsr _updateLMSValues
	rts
.endproc 


; void unpackbits(UInt8 *dest, const UInt8 *source);
.export _unpackbits
.proc _unpackbits
	; Unpacks data compressed with PackBits data from ptr2 into ptr1.
	; * On entry: ptr2 = PackBits-compressed data
	; * On return: ptr1 points at end of written data
	.importzp ptr1, ptr2
	.import popptr1

	src = ptr2 
	sta ptr2 
	stx ptr2+1

	dest = ptr1  		; start on byte 3 of display list area
	jsr popptr1 

	ldy #0 				; Y is always zero
	loop_header: 
		lda (src),Y 
		bmi repeated_data
	literal_data:
		jsr inc_src 	; src += 1
		tax 
		inx 
		loop_literal:
			lda (src),Y 
			sta (dest),Y 
			jsr inc_src 
			jsr inc_dest 
			dex 
			bne loop_literal
		jmp loop_header 
	repeated_data:
		cmp #128 		; 128 is terminator, which is different from 
		beq return 		; PackBits specification.
		jsr inc_src 	; src += 1
		eor #$FF
		clc 
		adc #2 
		tax 
		lda (src),Y 
		jsr inc_src 
		loop_repeated:
			sta (dest),Y 
			jsr inc_dest 
			dex 
			bne loop_repeated
		jmp loop_header 
	return:
		rts

	inc_src: 			; increment 16-bit src value
		inc src 
		bne @skip_msb 
			inc src+1
		@skip_msb:
		rts 
	inc_dest:			; increment 16-bit dest value
		inc dest 
		bne @skip_msb 
			inc dest+1
		@skip_msb:
		rts 
.endproc 


.proc _updateLMSValues
	; Replace the placeholder LMS values with pointers to actual screen data.
	; The first byte after LMS instruction is whether to use SAVMSC or TXTMSC.
	; The second byte is the number of rows to offset.

	.importzp tmp1 
	.import _setSavadrToTextCursor

	dl = ADRESS
	lda SDLSTL
	sta dl
	lda SDLSTL+1
	sta dl+1

	ldy #0
	sty COLCRS 

	loop: 
		lda (dl),Y 
		iny  
		beq return 	; if Y==0: wrapped around
		cmp #DL_JVB 
		beq set_jvb_address	; JVB = end of display list
		tax 
		and #$0F
		beq loop  		; skip blank line instructions
		txa 
		and #DL_LMS 
		beq loop
	found_lms:
		lda (dl),Y 
		iny 
		pha 
		lda (dl),Y
		sta ROWCRS 
		pla 
		bne text_ptr
	graphics_ptr:
		jsr _setSavadrToGraphicsCursor ; uses sreg, ptr1
		jmp set_lms_address
	text_ptr:
		jsr _setSavadrToTextCursor ; uses sreg, ptr1
	set_lms_address:
		dey 
		lda SAVADR 
		sta (dl),Y
		iny 
		lda SAVADR+1
		sta (dl),Y 
		iny 
		jmp loop 
	set_jvb_address:
		lda SDLSTL 
		sta (dl),Y 
		iny 
		lda SDLSTL+1
		sta (dl),Y
	return:
		rts 
.endproc


; extern void fadeOutColorTable(void);
.export _fadeOutColorTable
.proc _fadeOutColorTable
	; uses tmp1, ptr1
	.importzp ptr1 
	.importzp tmp1 
	.importzp dliColorTable
	.import _colorCyclingEnable

	; Disable player color cycling to avoid conflicts
	lda #0
	sta _colorCyclingEnable

	fadeAmount = tmp1  			; value passed to applyColorFade

	ldx #2 						; index = amount to fade, loop from 2 to <16
	loop_frame:
		stx fadeAmount
		ldy #0
		loop_color:
			lda PCOLR0,Y
			jsr _applyColorFade
			sta PCOLR0,Y
			iny 
			cpy #12
			bne loop_color

		lda dliColorTable+1
		beq skip_dliColorTable
		ldy #48
		loop_dli_color:
			lda (dliColorTable),Y
			jsr _applyColorFade 
			sta (dliColorTable),Y 
			dey 
			bne loop_dli_color
		skip_dliColorTable:

		lda #2
		jsr _delayTicks 		; delay 2 frames

		inx	 	 				; next index += 2
		inx 
		cpx #16
		bcc loop_frame
	rts
.endproc


; extern void fadeInColorTable(const UInt8 *colorTable);
.export _fadeInColorTable
.proc _fadeInColorTable
	; uses tmp1, ptr1
	.importzp ptr1 
	.importzp tmp1 

	sta ptr1
	stx ptr1+1

	ldx #16 					; tmp1 = amount to fade, loop from 14 to 0
	loop_frame:
		dex 
		dex 
		stx tmp1

		ldy #0
		loop_color:
			lda (ptr1),Y
			jsr _applyColorFade
			sta PCOLR0,Y
			iny 
			cpy #12
			bne loop_color

		lda #2
		jsr _delayTicks 		; delay 2 frames

		cpx #0
		bne loop_frame
	rts
.endproc


.proc _applyColorFade
	; on entry: A = color, tmp1 = amount to fade
	.importzp tmp1 

	pha 
	and #$0F	; A = lum
	cmp tmp1 	; if lum < amount: color = 0
	pla
	bcc return_zero
	subtract_color:
		sbc tmp1
		rts 
	return_zero:
		lda #0
		rts 
.endproc


; extern void loadColorTable(UInt8 *colors);
.export _loadColorTable
.proc _loadColorTable
	.importzp ptr1 

	sta ptr1 
	stx ptr1+1
	cpx #0
	beq zero_out
	copy_colors:
		ldy #0 
	loop_copy_colors:
		lda (ptr1),Y
		sta PCOLR0,Y
		iny 
		cpy #12 
		bne loop_copy_colors
	rts

	zero_out:
		ldy #0 
	loop_zero_out:
		lda #0
		sta PCOLR0,Y
		iny 
		cpy #12 
		bne loop_zero_out
	rts
.endproc


.export _sizeBarChart
.proc _sizeBarChart  	; uses sreg, ptr1, ptr4
	; Given hp and maxHp, calculate the width and fill values for a bar chart.
	; * On entry: 
	; A: hp
	; X: maxHp
	; * Sets values in:
	; DELTAC: width of bar chart
	; COLINC: width of filled portion of chart
	.importzp sreg, ptr1, ptr4
	.import udiv16

	pha 		; push hp
	maxHp = ptr4
	stx maxHp  	; 
	lda #0
	sta maxHp+1 

	set_width:
		txa 
		lsr a 
		cmp #36 
		bcc @skip_limit
		lda #36 
		@skip_limit:
		sta DELTAC 

	set_fill:
		tax 				; put width in X
		pla 	 			; pull hp off setack 
		jsr _multiplyAXtoPtr1 ; n = hp * width
		jsr udiv16 			;  n / maxHp => (ptr1, sreg)

		ldx ptr1  			; fill = n / maxHp
		lda sreg  			; if sreg != 0: inc fill, to round it up
		beq set_colinc 
		inx
	set_colinc:
		stx COLINC 
	rts
.endproc


; int drawImage(const DataBlock *image, UInt8 rowOffset, UInt8 rowCount);
.export _drawImage
.proc _drawImage
	.importzp ptr1
	.import popa, popptr1, pushax
	.import _multiplyAXtoPtr1
	.import _addAToPtr1
	.import _setSavadrToGraphicsCursor
	.import _uncompress

	get_rowCount:
	destLen = ENDPT  			; using ENDPT for its 16-bit size
		ldx #ROW_BYTES 			; A already has rowCount
		jsr _multiplyAXtoPtr1
		lda ptr1 
		sta destLen 
		lda ptr1+1 
		sta destLen+1 

	get_rowOffset:
		jsr popa
		sta ROWCRS
		lda #0
		sta COLCRS 
		jsr _setSavadrToGraphicsCursor

	get_image:
	image = ptr1 
		jsr popptr1

	pha_sourceLen:
		ldy #0
		lda (image),Y 			; LSB
		pha 
		iny 
		lda (image),Y  			; MSB
		pha

	push_dest: 
		ldx SAVADR+1
		lda SAVADR
		jsr pushax

	push_destLen:
		ldx #destLen+1 			; pushing pointer to destLen
		lda #destLen 
		jsr pushax 
		
	push_source:
		lda #2 
		jsr _addAToPtr1
		ldx ptr1+1
		lda ptr1 
		jsr pushax 

	push_sourceLen:
		pla 
		tax 
		pla 
	jsr _uncompress ; int uncompress (char* dest, unsigned* destLen, const char* source, unsigned sourceLen);
	rts
.endproc 

; void drawBarChart(void);
.export _drawBarChart
.proc _drawBarChart 
	; on entry: 
	; SAVADR: screen row pointer
	; COLCRS: x-position of bar chart.
	; DELTAC: width of bar chart
	; COLINC: width of filled portion of chart
	filled = COLINC
	width = DELTAC

	; Loop over pixels
	ldx #0
	loop: 
		lda #1 			; value = 1
		cpx filled 		; if X >= filled: x += 1
		adc #0
		eor #3

		jsr _setPixel

		inc COLCRS
		inx 
	while: 
		cpx width
		bne loop

	rts
.endproc

.proc _setPixel
	; on entry: SAVADR points to screen row, COLCRS is x-position, A=pixel value

	and #3 				; Use BUFSTR to store the shifted pixel value
	sta BUFSTR

	lda #3 				; Use BITMSK as the bit mask
	sta BITMSK

	lda COLCRS 			; Get the low 2-bits of cursor column
	and #3
	tay 

	jmp while_shift		; shift left, where position 0 requires 3 shifts
	loop_shift: 
		asl BUFSTR ; 5
		asl BUFSTR ; 5
		asl BITMSK
		asl BITMSK
		iny 

	while_shift:
		cpy #3
		bne loop_shift

	lda COLCRS  		; divided cursor column by 4 to get the byte offset
	lsr a 
	lsr a 
	tay

	lda BITMSK 			; invert the bit mask
	eor #$FF
	sta BITMSK

	lda (SAVADR),Y 		; mask out the pixel and apply the new value
	and BITMSK
	ora BUFSTR
	sta (SAVADR),Y

	rts
.endproc 

; extern void zeroOut16(UInt8 *ptr, UInt16 length);
.export _zeroOut16
.proc _zeroOut16
	.importzp sreg, ptr1 
	.import popsreg
	.import _addSregToPtr1

	length = ptr1 
	sta length			; ptr1 = parmeter 'length'
	stx length+1

	ptr = sreg
	jsr popsreg			; sreg = parameter 'ptr'

	jsr _addSregToPtr1	; set length to point at end of area to zero out

	ldy #0
	loop:
		lda #0
		sta (ptr),Y

		inc ptr 
		bne while
		inc ptr+1
	while:
		lda ptr 
		cmp length
		bne loop
		lda ptr+1
		cmp length+1
		bne loop
	rts 
.endproc 


; void zeroOut8(UInt8 *ptr, UInt8 length);
.export _zeroOut8
.proc _zeroOut8
	.import popptr1 

	pha		 			; get parmeter 'length'
	jsr popptr1			; get parameter 'ptr'
	pla 
	tay 
	jsr _zeroOutYAtPtr1
	rts 
.endproc


.export _zeroOutYAtPtr1
.proc _zeroOutYAtPtr1
	.importzp ptr1

	lda #0
	loop:
		dey
		sta (ptr1),Y
		bne loop
	rts 
.endproc


.export _setSavadrToGraphicsCursor
.proc _setSavadrToGraphicsCursor 
	; Stores cursor address in SAVADR.
	; Set DINDEX to zero for normal mode, nonzero for map view mode.
	; Calls _multiplyAXtoPtr1 (uses sreg, ptr1)
	.importzp ptr1

	lda DINDEX 
	bne tile_mode
	normal_mode:
		ldx #40 		; 40 bytes per row
		jmp mode_done
	tile_mode:
		ldx #24
	mode_done:

	lda ROWCRS			; ptr1 = y * row_bytes
	jsr _multiplyAXtoPtr1

	clc 				; SAVADR = ptr1 + TXTMSC
	lda ptr1
	adc SAVMSC 
	sta SAVADR
	lda ptr1+1
	adc SAVMSC+1
	sta SAVADR+1

	lda COLCRS
	jsr _addAToSavadr

	rts 
.endproc

.export _addAToSavadr
.proc _addAToSavadr
	clc 				; SAVADR += A
	adc SAVADR 
	sta SAVADR
	bcc @skip_msb 
		inc SAVADR+1
	@skip_msb:
	rts 
.endproc 

; extern void delayTicks(UInt8 ticks);
.export _delayTicks
.proc _delayTicks
	clc
	adc RTCLOK_LSB
	loop:
		cmp RTCLOK_LSB
		bne loop 
	rts
.endproc


.rodata
customTiles:
	.byte $00, $00, $00, $00, $00, $00, $00, $00 ; Blank
	.byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF ; Solid / Desert

	.byte $FF, $F7, $FF, $BF, $FD, $FF, $EF, $FF ; Plains
	.byte $7E, $7E, $00, $7E, $7E, $00, $7E, $7E ; BridgeH
	.byte $00, $DB, $DB, $DB, $DB, $DB, $DB, $00 ; BridgeV
	.byte $FF, $F7, $E3, $F7, $C1, $F7, $80, $F7 ; Forest
	.byte $FE, $FF, $FB, $FF, $EF, $FF, $BF, $FF ; Shallows
	.byte $CE, $B5, $5D, $73, $CE, $BA, $AD, $73 ; Water
	.byte $F7, $E3, $C1, $80, $FF, $BF, $1F, $0E ; Mountains

	.byte $DF, $DF, $DF, $00, $FD, $FD, $FD, $00 ; Brick
	.byte $FE, $FF, $BB, $FF, $EF, $FF, $BB, $FF ; Floor
	.byte $24, $3C, $24, $3C, $24, $3C, $24, $00 ; Ladder
	.byte $3C, $FF, $00, $E7, $FF, $FF, $FF, $00 ; Chest
	.byte $C0, $DF, $9F, $BF, $3F, $7F, $7F, $00 ; House1
	.byte $07, $E3, $EB, $C9, $DD, $9C, $BE, $00 ; House2
	.byte $7F, $7F, $7F, $4C, $4C, $7F, $7F, $00 ; House3
	.byte $6D, $6D, $6D, $12, $6D, $6D, $6D, $00 ; House4

	.byte $3C, $18, $18, $2C, $5E, $7E, $3C, $00 ; Potion
	.byte $04, $04, $0C, $0C, $1C, $7C, $78, $30 ; Fang

	.byte $00, $00, $22, $1C, $1C, $18, $22, $00 ; Castle
	.byte $00, $00, $28, $00, $28, $00, $00, $00 ; Town
	.byte $FF, $D3, $89, $C9, $AC, $93, $CD, $F7 ; Village
	.byte $FF, $FF, $FF, $E7, $E3, $F1, $F8, $FC ; Monument
	.byte $FF, $FF, $FF, $E7, $E7, $E7, $FF, $FF ; Cave
	.byte $80, $80, $80, $80, $80, $80, $80, $00 ; HouseDoor

	customTilesLength = 26*8
.code


.proc _initFont
	; uses ptr1, ptr2
	.importzp ptr1, ptr2

	; Set the map view to use characters starting at fontPage + 2. It seems that the text box area ignores the bottom 2 bits of CHBAS, allowing for regular characters starting at fontPage.
	customFont = ptr1 
		sta customFont+1 		; MSB of ptr1 = fontPage
		clc  				; CHBAS = fontPage + 2
		adc #2
		sta CHBAS 			
		lda #0 				; LSB of ptr1 = 0
		sta customFont 

	; Copy entire ROM font into custom font area.
	romFont = ptr2 
	lda #$00 			; ptr2: ROM font
	sta romFont
	lda #$E0 
	sta romFont+1
	ldy #0 
	loop_rom: 			
		lda (romFont),Y		; copy ROM font into custom font area
		sta (customFont),Y
		iny
		bne loop_rom
		inc customFont+1
		inc romFont+1
		lda romFont+1
		cmp #$E4
		bne loop_rom

		; Copy our custom tiles into the control character area starting at fontPage + 2.
		lda customFont+1
		sec 
		sbc #2  			; at this point, ptr1 is at end of font area, so subtract 2 
		sta customFont+1
		ldy #0
	loop_custom:
		lda customTiles,Y
		sta (customFont),Y
		iny
		cpy #customTilesLength
		bne loop_custom
	rts
.endproc


; extern void initGraphics(void);
.export _initGraphics
.proc _initGraphics
	.importzp ptr1

	.import _initVBI
	.import _initSprites

	; Turn off screen during init and leave it off for main to turn back on.
	lda #0
	sta SDMCTL

	; Set color table to all black
	ldx #0
	jsr _loadColorTable 

	; Init VBI
	jsr _initVBI

	; Init Display List
	dispList = ptr1
	lda RAMTOP 
	sec 
	sbc #12 
	sta dispList+1
	sta SDLSTL+1	; set SDLSTL to point at 12 pages below ramtop (3 KB)
	sta SAVMSC+1	; screen memory is on same page

	ldx #0			; set LSB
	stx dispList
	stx SDLSTL
	ldx #$80
	stx SAVMSC		; screen memory is 128 bytes after display list

	; Set up lines common to all display lists
	ldy #0
	lda #DL_BLK8
	sta (dispList),Y
	iny
	sta (dispList),Y
	iny
	lda #DL_BLK4
	sta (dispList),Y

	; Init Sprites
	ldx #0
	stx TXTMSC 			; Text window goes on same page as sprites
	lda RAMTOP
	sec 
	sbc #16
	sta TXTMSC+1 		
	jsr _initSprites

	; Init Font
	lda RAMTOP
	sec 
	sbc #20
	jsr _initFont

	; Use scrolling to center the odd number of tiles
	lda #4
	sta HSCROL

	rts
.endproc

