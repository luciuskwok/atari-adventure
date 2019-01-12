; graphics_.asm

.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4
.import 	popa, popptr1, popsreg, pushax
.import 	udiv16
.import 	_inflatemem

.import _initVBI
.import _initSprites

.import _mapViewDLI
.import _battleViewDLI
.import _infoViewDLI

.import _multiplyAXtoPtr1
.import _addSregToPtr1, _addAToPtr1
.import _setSavadrToTextCursor

.import _setCursorColorCycling

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
	anticOptions = $2E 	; normal playfield, enable players & missiles, enable DMA
	enableDLI    = $C0  ; enable VBI + DLI
	mode = tmp1 		; use tmp1 for mode

	sta mode  			; save mode in tmp1
	
	lda #0 				; Turn off screen
	sta SDMCTL
	;sta DMLCTL
	lda #$40 			; disable DLI, leave VBI enabled
	sta NMIEN

	lda #1
	jsr _delayTicks

	lda mode 			; if mode == ScreenModeInfo: init Info screen
	cmp #ScreenModeInfo
	bne non_info_screen

	init_info:
		jsr _initInfoViewDisplay

		lda #<_infoViewDLI
		sta VDSLST
		lda #>_infoViewDLI
		sta VDSLST+1

		jmp enable_screen

	non_info_screen: 		; else: set SAVMSC to textWindow
		lda mode
		cmp #ScreenModeMap
		beq init_map
		cmp #ScreenModeDialog
		beq init_dialog
		cmp #ScreenModeBattle
		beq init_battle
		rts 				; default: screen off

	init_map:
		jsr _initMapViewDisplay

		lda #<_mapViewDLI
		sta VDSLST
		lda #>_mapViewDLI
		sta VDSLST+1

		jmp enable_screen

	init_dialog:
		jsr _initDialogViewDisplay

		lda #0
		sta VDSLST
		sta VDSLST+1

		jmp enable_screen

	init_battle:
		jsr _initBattleViewDisplay

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


.proc _initMapViewDisplay
	.rodata
	packedMapDL: ; display list in PackBits format
		.byte   3-1,   DL_TILE|DL_DLI|DL_HSCROL|DL_LMS, LMS_GR, 0 ; tile, 9 rows
		.byte  -8+257, DL_TILE|DL_DLI|DL_HSCROL

		.byte  19-1,   DL_BLK6, DL_TEXT|DL_DLI|DL_LMS, LMS_TXT, 0 ; text, 3 rows
		.byte          DL_TEXT, DL_TEXT|DL_DLI

		.byte          DL_BLK1, DL_RASTER			 	; HP bar
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 3 	; add 3 rows to TXTMSC
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 3, DL_BLK1|DL_DLI

		.byte          DL_BLK7, DL_TEXT_SPACER, DL_TEXT, DL_TEXT_SPACER|DL_DLI
		.byte 128 ; terminator

	.code 
	lda #<packedMapDL
	ldx #>packedMapDL
	jsr _unpackDisplayList
	textHeight = 7
		lda #textHeight 	; update text window height
		sta BOTSCR
	rts
.endproc


.proc _initDialogViewDisplay
	.rodata
	packedDialogDL: ; display list in PackBits format
		.byte   3-1,   DL_RASTER|DL_LMS, LMS_GR, 0 ; raster, 72 rows
		.byte -71+257, DL_RASTER 
		.byte   3-1,   DL_TEXT|DL_LMS, LMS_TXT, 0 ; text, 7 rows
		.byte  -6+257, DL_TEXT 
		.byte 128 ; terminator

	.code 
	lda #<packedDialogDL
	ldx #>packedDialogDL
	jsr _unpackDisplayList 		; returns end of DL in ptr1
	textHeight = 7
		lda #textHeight 	; update text window height
		sta BOTSCR
	rts
.endproc


.proc _initBattleViewDisplay
	.rodata
	packedBattleDL: ; display list in PackBits format
		.byte   3-1,   DL_RASTER|DL_LMS, LMS_GR, 0 	; raster, 48 rows
		.byte -46+257, DL_RASTER 
		.byte  13-1,   DL_RASTER|DL_DLI

		.byte          DL_BLK1
		.byte          DL_RASTER				 	; enemy HP bar
		.byte          DL_RASTER|DL_LMS, LMS_GR, 48
		.byte          DL_RASTER|DL_LMS, LMS_GR, 48
		.byte          DL_BLK5

		.byte          DL_TEXT|DL_LMS, LMS_TXT, 0 	; text, 7 rows
		.byte  -6+257, DL_TEXT 

		.byte  13-1,   DL_BLK1
		.byte          DL_RASTER				 	; player HP bars
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 7
		.byte          DL_RASTER|DL_LMS, LMS_TXT, 7
		.byte          DL_BLK1|DL_DLI

		.byte          DL_BLK8

		.byte          DL_RASTER|DL_LMS, LMS_GR, 49	; raster, 10 rows
		.byte  -9+257, DL_RASTER 

		.byte 128 ; terminator

	.code 
	lda #<packedBattleDL
	ldx #>packedBattleDL
	jsr _unpackDisplayList 		; returns end of DL in ptr1
	textHeight = 7
		lda #textHeight 	; update text window height
		sta BOTSCR
	rts
.endproc


.proc _initInfoViewDisplay
	.rodata
	packedInfoDL: ; display list in PackBits format
		.byte   3-1,   DL_RASTER|DL_LMS, LMS_GR, 0 ; raster, 24 rows
		.byte -21+257, DL_RASTER 
		.byte   4-1,   DL_RASTER|DL_DLI, DL_RASTER|DL_DLI
		.byte          DL_TEXT|DL_DLI, DL_BLK1	; chara name
		.byte -11+257, DL_TEXT 					; chara stats
		.byte   4-1,   DL_TEXT|DL_DLI, DL_BLK8 
		.byte          DL_TEXT|DL_DLI, DL_BLK1 ; "Items"
		.byte  -3+257, DL_TEXT 					; items body
		.byte   1-1,   DL_TEXT|DL_DLI 			; last line DLI
		.byte 128 ; terminator

	.code 
	lda #<packedInfoDL
	ldx #>packedInfoDL
	jsr _unpackDisplayList 		; returns end of DL in ptr1
	textHeight = 18
		lda #textHeight 	; update text window height
		sta BOTSCR
	rts
.endproc


.proc _unpackDisplayList 
	; Reads PackBits data into display list area.
	; * On entry: AX = PackBits-compressed data
	; * uses ptr1, ptr2, tmp1
	; * On return: ptr1 points at end of written data
	
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

	jsr _unpackbits
	jsr _writeDisplayListEndInternal
	jsr _updateLMSValues
	rts
.endproc 


.proc _unpackbits
	; Unpacks data compressed with PackBits data from ptr2 into ptr1.
	; * On entry: ptr2 = PackBits-compressed data
	; * On return: ptr1 points at end of written data
	; * uses ptr1, ptr2, tmp1

	src = ptr2 
	dest = ptr1  		; start on byte 3 of display list area
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

	dl = tmp2
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
		beq return	; JVB = end of display list
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
		jsr _setSavadrToGraphicsCursor
		jmp set_lms_address
	text_ptr:
		jsr _setSavadrToTextCursor
	set_lms_address:
		dey 
		lda SAVADR 
		sta (dl),Y
		iny 
		lda SAVADR+1
		sta (dl),Y 
		iny 
		jmp loop 
	return:
		rts 
.endproc

.proc _initDisplayListVarsInternal
	lda SDLSTL 
	sta ptr1
	lda SDLSTL+1
	sta ptr1+1
	ldy #3
	rts
.endproc


.proc _writeDisplayListLinesInternal
	; on entry: A=mode, X=count, Y=index, ptr1=DL_ptr, ptr2=screen_ptr
	pha 				; push mode value on stack for later

	ora #DL_LMS			; add LMS to first line
	sta (ptr1),Y
	iny

	lda ptr2  			; use ptr2 as pointer to screen memory
	sta (ptr1),Y
	iny 

	lda ptr2+1
	sta (ptr1),Y
	iny 
	dex 

	pla 				; pull mode value from stack
	jmp while
	loop:
		sta (ptr1),Y
		iny 
		dex
	while:
		cpx #0
		bne loop
	rts
.endproc


.proc _writeDisplayListBarChartInternal
	; on entry: Y=index, ptr1=DL_ptr, ptr2=screen_ptr
	lda #DL_BLK1
	sta (ptr1),Y
	iny 

	ldx #3
	loop:
		lda #DL_RASTER|DL_LMS
		sta (ptr1),Y
		iny 

		lda ptr2
		sta (ptr1),Y
		iny 

		lda ptr2+1
		sta (ptr1),Y
		iny 
		
		dex
		bne loop

	lda #DL_BLK1
	sta (ptr1),Y
	iny 

	rts
.endproc


.proc _applyTrailingDLI
	dey 
	lda (ptr1),Y
	ora #DL_DLI
	sta (ptr1),Y
	iny
	rts 
.endproc 


.proc _writeDisplayListEndInternal
	; on entry: ptr1=DL_ptr
	ldy #0
	lda #DL_JVB
	sta (ptr1),Y
	iny 
	lda SDLSTL
	sta (ptr1),Y
	iny 
	lda SDLSTL+1
	sta (ptr1),Y
	rts
.endproc


; extern void fadeOutColorTable(void);
.export _fadeOutColorTable
.proc _fadeOutColorTable
	; uses tmp1

	; Disable player color cycling to avoid conflicts
	lda #0
	ldx #0
	jsr _setCursorColorCycling

	ldx #2 						; tmp1 = amount to fade, loop from 2 to <16
	loop_frame:
		stx tmp1
		ldy #0
		loop_color:
			lda PCOLR0,Y
			jsr _applyColorFade
			sta PCOLR0,Y
			iny 
			cpy #12
			bne loop_color

		lda #2
		jsr _delayTicks 		; delay 2 frames

		inx	 	 				; next tmp1 += 2
		inx 
		cpx #16
		bcc loop_frame
	rts
.endproc


; extern void fadeInColorTable(const UInt8 *colorTable);
.export _fadeInColorTable
.proc _fadeInColorTable
	; uses tmp1, ptr1
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
	; uses ptr1 
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


; void drawImage(const DataBlock *image, UInt8 rowOffset);
.export _drawImage
.proc _drawImage 		; uses ptr1
	sta ROWCRS
	lda #0
	sta COLCRS
	jsr _setSavadrToGraphicsCursor

	jsr popptr1 

	lda SAVADR
	ldx SAVADR+1
	jsr pushax

	clc 
	lda ptr1 
	adc #2
	ldx ptr1+1
	bcc @skip_msb 
	inx 
	@skip_msb:

	jmp _inflatemem
	;rts 
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
	; uses AY

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
	pha		 			; get parmeter 'length'
	jsr popptr1			; get parameter 'ptr'
	pla 
	tay 
	jsr _zeroOutYAtPtr1
	rts 
.endproc


.export _zeroOutYAtPtr1
.proc _zeroOutYAtPtr1
	lda #0
	loop:
		dey
		sta (ptr1),Y
		bne loop
	rts 
.endproc


.export _setSavadrToGraphicsCursor
.proc _setSavadrToGraphicsCursor 
	; Stores cursor address in SAVADR
	; Calls _multiplyAXtoPtr1 (uses sreg, ptr1)
	.importzp 	ptr1

	lda ROWCRS			; ptr1 = y * row_bytes
	ldx #ROW_BYTES
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
	; uses ptr1, ptr2, TBD

	; Set the map view to use characters starting at fontPage + 2. It seems that the text box area ignores the bottom 2 bits of CHBAS, allowing for regular characters starting at fontPage.
	sta ptr1+1 			; MSB of ptr1 = fontPage
	clc  				; CHBAS = fontPage + 2
	adc #2
	sta CHBAS 			
	lda #0 				; LSB of ptr1 = 0
	sta ptr1 

	; Copy entire ROM font into custom font area.
	lda #$00 			; ptr2: ROM font
	sta ptr2
	lda #$E0 
	sta ptr2+1
	ldy #0 
	loop_rom: 			
		lda (ptr2),Y		; copy ROM font into custom font area
		sta (ptr1),Y
		iny
		bne loop_rom
		inc ptr1+1
		inc ptr2+1
		lda ptr2+1
		cmp #$E4
		bne loop_rom

		; Copy our custom tiles into the control character area starting at fontPage + 2.
		lda ptr1+1
		sec 
		sbc #2  			; at this point, ptr1 is at end of font area, so subtract 2 
		sta ptr1+1
		ldy #0
	loop_custom:
		lda customTiles,Y
		sta (ptr1),Y
		iny
		cpy #customTilesLength
		bne loop_custom
	rts
.endproc


; extern void initGraphics(void);
.export _initGraphics
.proc _initGraphics
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
