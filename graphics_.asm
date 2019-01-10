; graphics_.asm

.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

.import _graphicsWindow
.import _textWindow

.import _mapViewDLI
.import _battleViewDLI
.import _infoViewDLI


; Memory locations
	RTCLOK_LSB = $14 		; LSB of internal clock
	SAVMSC 	  = $58 		; Pointer to screen memory
	VDSLST    = $0200 		; Pointer to current display list handler
	SDMCTL    = $022F 		; Shadow register for ANTIC options
	SDLSTL    = $0230		; Pointer to display list
	PCOLR0    = $02C0		; Player 0 color
	CHBAS     = $02F4
	DMACTL    = $D400 		; ANTIC options
	NMIEN     = $D40E 		; ANTIC NMI enable

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

	ROW_BYTES = 40

; Screen modes
	ScreenModeMap    = 1
	ScreenModeDialog = 2
	ScreenModeBattle = 3
	ScreenModeInfo   = 4


; extern void __fastcall__ setScreenMode(UInt8 mode);
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

	lda RTCLOK_LSB 		; Wait for vertical blank
wait_vbi:
	cmp RTCLOK_LSB
	beq wait_vbi

	lda mode 			; if mode == ScreenModeInfo: init Info screen
	cmp #ScreenModeInfo
	bne non_info_screen

init_info:
	lda _graphicsWindow  ; set SAVMSC to graphcsWindow
	sta SAVMSC
	lda _graphicsWindow+1
	sta SAVMSC+1

	jsr _initInfoViewDisplay

	lda #<_infoViewDLI
	sta VDSLST
	lda #>_infoViewDLI
	sta VDSLST+1

	jmp enable_screen

non_info_screen: 		; else: set SAVMSC to textWindow
	lda _textWindow 
	sta SAVMSC
	lda _textWindow+1
	sta SAVMSC+1

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
	jsr _initDisplayListVarsInternal

	; Use graphicsWindow screen memory for tiles
	lda _graphicsWindow 		
	sta ptr2
	lda _graphicsWindow+1
	sta ptr2+1
	ldx #9
	lda #DL_TILE|DL_DLI|DL_HSCROL
	jsr _writeDisplayListLinesInternal

	; Add 6 blank lines
	lda #DL_BLK6
	sta (ptr1),Y
	iny

	; Use textWindow screen memory for text box
	lda _textWindow  			
	sta ptr2
	lda _textWindow+1
	sta ptr2+1
	ldx #3
	lda #DL_TEXT|DL_DLI
	jsr _writeDisplayListLinesInternal

	; Continue using textWindow for bar chart
	lda ptr2
	clc 
	adc #3*ROW_BYTES
	sta ptr2
	lda ptr2+1
	adc #0
	sta ptr2+1
	jsr _writeDisplayListBarChartInternal

	; Add DLI

	; Party stats
	lda #DL_BLK3|DL_DLI
	sta (ptr1),Y
	iny

	lda #DL_BLK4
	sta (ptr1),Y
	iny

	lda #DL_TEXT_SPACER
	sta (ptr1),Y
	iny

	lda #DL_TEXT
	sta (ptr1),Y
	iny

	lda #DL_TEXT_SPACER|DL_DLI
	sta (ptr1),Y
	iny

	jsr _writeDisplayListEndInternal
	rts
.endproc


.proc _initDialogViewDisplay
	jsr _initDisplayListVarsInternal

	; Use graphicsWindow screen memory for raster
	lda _graphicsWindow 		
	sta ptr2
	lda _graphicsWindow+1
	sta ptr2+1
	ldx #72
	lda #DL_RASTER
	jsr _writeDisplayListLinesInternal

	; Use textWindow screen memory for text box
	lda _textWindow  			
	sta ptr2
	lda _textWindow+1
	sta ptr2+1
	ldx #7
	lda #DL_TEXT|DL_DLI
	jsr _writeDisplayListLinesInternal

	jsr _writeDisplayListEndInternal
	rts
.endproc


.proc _initBattleViewDisplay
	rasterHeight = 48

	jsr _initDisplayListVarsInternal

	; Use graphicsWindow screen memory for raster
	lda _graphicsWindow 		
	sta ptr2
	lda _graphicsWindow+1
	sta ptr2+1
	ldx #rasterHeight
	lda #DL_RASTER
	jsr _writeDisplayListLinesInternal

	jsr _applyTrailingDLI

	; Continue graphicsWindow for enemy HP bar chart
	clc
	lda ptr2
	adc #<(rasterHeight*40) ; 48 * 40 = 1920 = $780
	sta ptr2 
	lda ptr2+1
	adc #>(rasterHeight*40)
	sta ptr2+1
	jsr _writeDisplayListBarChartInternal

	; Add 4-line spacer
	lda #DL_BLK4
	sta (ptr1),Y
	iny

	; Use textWindow screen memory for text box
	lda _textWindow  			
	sta ptr2
	lda _textWindow+1
	sta ptr2+1
	ldx #7
	lda #DL_TEXT
	jsr _writeDisplayListLinesInternal

	; Continue using textWindow for bar chart
	lda ptr2
	clc 
	adc #$18  ; 7 * 40 = 280 = $118
	sta ptr2
	lda ptr2+1
	adc #$01
	sta ptr2+1
	jsr _writeDisplayListBarChartInternal

	; Add 4-line spacer with DLI
	lda #DL_BLK4|DL_DLI
	sta (ptr1),Y
	iny

	; Add 4-line spacer
	lda #DL_BLK4
	sta (ptr1),Y
	iny

	; Return to raster graphics for button area
	clc
	lda _graphicsWindow 
	adc #<((1+rasterHeight)*40)
	sta ptr2
	lda _graphicsWindow+1
	adc #>((1+rasterHeight)*40)
	sta ptr2+1
	ldx #10
	lda #DL_RASTER
	jsr _writeDisplayListLinesInternal

	jsr _writeDisplayListEndInternal
	rts
.endproc


; extern void __fastcall__ initInfoViewDisplay(void);
.export _initInfoViewDisplay
.proc _initInfoViewDisplay
	rasterHeight = 24

	jsr _initDisplayListVarsInternal

	; Use graphicsWindow screen memory for both raster and text
	lda _graphicsWindow 		
	sta ptr2
	lda _graphicsWindow+1
	sta ptr2+1
	ldx #rasterHeight
	lda #DL_RASTER
	jsr _writeDisplayListLinesInternal

	jsr _applyTrailingDLI

	; Chara name
	lda #DL_TEXT|DL_DLI
	sta (ptr1),Y
	iny

	lda #DL_BLK2
	sta (ptr1),Y
	iny

	; Chara stats: 12 lines 
	lda #DL_TEXT
	ldx #11
loop1:
	sta (ptr1),Y
	iny
	dex 
	bne loop1

	lda #DL_TEXT|DL_DLI
	sta (ptr1),Y
	iny

	lda #DL_BLK4
	sta (ptr1),Y
	iny

	; Party stats
	lda #DL_TEXT|DL_DLI
	sta (ptr1),Y
	iny

	lda #DL_TEXT
	ldx #4
loop2:
	sta (ptr1),Y
	iny
	dex 
	bne loop2

	jsr _writeDisplayListEndInternal
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
	; on entry: ptr1=DL_ptr, Y=index
	lda #DL_JVB
	sta (ptr1),Y
	iny 

	lda SDLSTL
	sta (ptr1),Y
	iny 

	lda SDLSTL+1
	sta (ptr1),Y
	iny 

	rts
.endproc


; extern void __fastcall__ loadColorTable(UInt8 *colors);
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


; extern void __fastcall__ initFont(UInt8 fontPage);
.export _initFont
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
