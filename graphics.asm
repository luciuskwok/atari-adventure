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
	ScreenModeInfo   = 2
	ScreenModeBattle = 3
	ScreenModeShop   = 4
	ScreenModeDialog = 5

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
		.byte   3-1,   DL_RASTER|DL_LMS, LMS_GR, 0 ; raster, 25 rows
		.byte -22+257, DL_RASTER 
		.byte   4-1,   DL_RASTER|DL_DLI, DL_RASTER|DL_DLI
		.byte          DL_TEXT|DL_DLI, DL_BLK1	; chara name
		.byte -11+257, DL_TEXT 					; chara stats
		.byte   5-1,   DL_TEXT|DL_DLI
		.byte          DL_BLK4, DL_RASTER
		.byte          DL_TEXT|DL_DLI, DL_BLK1 	; "Items"
		.byte  -3+257, DL_TEXT 					; items body
		.byte   2-1,   DL_TEXT|DL_DLI 			; last line DLI
		.byte          DL_JVB
		.byte 128 ; terminator

	packedDialogDL: ; display list in PackBits format
		.byte   3-1,   DL_RASTER|DL_DLI|DL_LMS, LMS_GR, 0 ; graphics, 48 rows
		.byte -47+257, DL_RASTER|DL_DLI
		.byte  21-1,   DL_BLK8
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_TEXT, DL_BLK2
		.byte          DL_BLK8|DL_DLI
		.byte          DL_JVB
		.byte 128 ; terminator

	packedShopDL: ; display list in PackBits format
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
	.import _dialogViewDLI

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
		cmp #ScreenModeShop
			beq init_shop
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

	init_shop:
		lda #<packedShopDL
		ldx #>packedShopDL
		jsr _unpackDisplayList

		lda #0
		sta VDSLST
		sta VDSLST+1

		jmp enable_screen

	init_dialog:
		lda #<packedDialogDL
		ldx #>packedDialogDL
		jsr _unpackDisplayList

		lda #<_dialogViewDLI
		sta VDSLST
		lda #>_dialogViewDLI
		sta VDSLST+1

		lda #9	 			; text window height
		sta BOTSCR

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


; int drawCompressedImage(const DataBlock *image, UInt8 rowOffset, UInt8 rowCount);
.export _drawCompressedImage
.proc _drawCompressedImage
	.importzp ptr1
	.import popa, popptr1, pushax
	.import _multiplyAXtoPtr1
	.import _addAToPtr1
	.import _setSavadrToGraphicsCursor
	.import _uncompress
	.import _mulax40

	get_rowCount:
	destLen = ENDPT  			; using ENDPT for its 16-bit size
		ldx #0 
		jsr _mulax40 
		sta destLen 
		stx destLen+1

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
		ldx #>destLen 			; pushing pointer to destLen
		lda #<destLen 
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

; void drawImage(const UInt8 *data);
.export _drawImage 
.proc _drawImage 
	; On entry:
	; * COLCRS: x-position of origin, in byte columns
	; * ROWCRS: y-position of origin
	; * DELTAC: image width, in number of bytes
	; * DELTAR: image height
	.importzp ptr2 
	.import _setSavadrToGraphicsCursor
	.import _addAToPtr2

	imageData = ptr2
		sta imageData 
		stx imageData+1 

	check_parameters:
		lda DELTAC  			; return if image width or height is zero
		beq return 
		lda DELTAR
		beq return 

	jsr _setSavadrToGraphicsCursor
	ldx DELTAR 	; X = rowIndex
	loop_row:
		ldy #0  ; Y = colIndex
		loop_col:
			lda (imageData),Y 
			sta (SAVADR),Y
			next_col:
				iny 
				cpy DELTAC 
				bcc loop_col 

		next_imageRow:
			lda DELTAC
			jsr _addAToPtr2

		next_screenRow:
			lda #ROW_BYTES
			jsr _addAToSavadr

		next_row:
			dex 
			bne loop_row 
	return:
		rts
.endproc

; void compositeImage(const UInt8 *data);
.export _compositeImage
.proc _compositeImage
	; On entry:
	; * COLCRS: x-position of origin, in byte columns
	; * ROWCRS: y-position of origin
	; * DELTAC: image width, in number of bytes
	; * DELTAR: image height
	; Uses:
	; * BITMSK for mask
	; * TMPCHR for value
	.importzp ptr2 
	.import _setSavadrToGraphicsCursor
	.import _addAToPtr2

	imageData = ptr2
		sta imageData 
		stx imageData+1 

	check_parameters:
		lda DELTAC  			; return if image width or height is zero
		beq return 
		lda DELTAR
		beq return 

	jsr _setSavadrToGraphicsCursor
	ldx DELTAR 	; X = rowIndex
	loop_row:
		ldy #0  ; Y = colIndex
		loop_col:
			lda (imageData),Y 
			sta TMPCHR

			apply_mask:
				and #$AA
				lsr a 
				sta BITMSK 

				lda TMPCHR 
				and #$55 
				asl a 
				ora BITMSK

				eor #$FF
				and (SAVADR),Y 

			apply_image:
				ora TMPCHR
				sta (SAVADR),Y

			next_col:
				iny 
				cpy DELTAC 
				bcc loop_col 

		next_imageRow:
			lda DELTAC
			jsr _addAToPtr2

		next_screenRow:
			lda #ROW_BYTES
			jsr _addAToSavadr

		next_row:
			dex 
			bne loop_row 
	return:
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

; void zeroOut16(UInt8 *ptr, UInt16 length);
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


; void setMemory8(UInt8 *ptr, UInt8 value, UInt8 length);
.export _setMemory8
.proc _setMemory8
	.importzp ptr1, tmp1, tmp2 
	.import popptr1 , popa

	length = tmp1
		sta length	; parameter "length"
	value = tmp2 
		jsr popa 	; parameter "value"
		sta value 
	jsr popptr1 
	ldy length 
	lda value
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
	.import _mulax40

	lda DINDEX 
	bne tile_mode
	normal_mode:
		lda ROWCRS 
		ldx #0 
		jsr _mulax40		; 40 bytes per row
		jmp mode_done
	tile_mode:
		ldx #24
		lda ROWCRS			; ptr1 = y * row_bytes
		jsr _multiplyAXtoPtr1
		lda ptr1 
		ldx ptr1+1
	mode_done:

	clc 				; SAVADR = ptr1 + TXTMSC
	adc SAVMSC 
	sta SAVADR
	txa 
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


; extern void initGraphics(void);
.export _initGraphics
.proc _initGraphics
	.importzp ptr1

	.import _initVBI
	.import _initSprites
	.import _setWaterTile
	.import customFontData

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
	; Font data is already loaded into memory using the Atari exe file format's ability to define separate segments to load into specific areas of memory.

	; Set the map view to use characters starting at CHBAS + 2. It seems that the text box area ignores the bottom 2 bits of CHBAS, allowing for regular characters starting at CHBAS.

	lda #>customFontData
	clc 
	adc #2
	sta CHBAS 			

	; Set water tile pointer for VBI
	tax 
	lda #6*8 ; tile numbers 6 and 7 are water
	jsr _setWaterTile

	; Use scrolling to center the odd number of tiles
	lda #4
	sta HSCROL

	rts
.endproc

