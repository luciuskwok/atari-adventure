; interrupts.asm

; Exports
.export _initVBI			; void __fastcall__ initVBI(void);
.export _mapViewDLI
.export _battleViewDLI
.export _infoViewDLI

; Imports
.import _soundVBI
.import _updateSoundSprites
.import _initSound
.import _dliSpriteData

; Include
.include "atari_memmap.asm"

; Constants
	VB_TIMER  = $0601	; general countdown timer that decrements every VBI

	vcountTopMargin = 13

.data  ; Globals 
	_colorCyclingEnable: .byte 0
	.export _colorCyclingEnable

.segment "EXTZP": zeropage
	dliColorTable: .word 0
	.exportzp dliColorTable

.code

.proc _initVBI		; 
	jsr _initSound

	ldy #<_immediateUserVBI
	ldx #>_immediateUserVBI
	lda #6			; 6=Immediate, 7=Deferred.
	jsr $E45C		; SETVBV: Y=LSB, X=MSB

	ldy #<_deferredUserVBI
	ldx #>_deferredUserVBI
	lda #7			; 6=Immediate, 7=Deferred.
	jsr $E45C		; SETVBV: Y=LSB, X=MSB

	rts
.endproc


; void setDliColorTable(UInt8 *ptr);
.export _setDliColorTable
.proc _setDliColorTable 
	sta dliColorTable
	stx dliColorTable+1
	rts
.endproc

; UInt8* getDliColorTable(void);
.export _getDliColorTable 
.proc _getDliColorTable 
	lda dliColorTable
	ldx dliColorTable+1 
	rts 
.endproc


.proc _immediateUserVBI
	jsr _soundVBI
	jmp SYSVBV			; jump to the OS immediate VBI routine
.endproc


.proc _deferredUserVBI	
	jsr _updateSoundSprites

	ldx VB_TIMER		; 4 ; sets Z flag if zero
	beq @skip_vb_timer	; 2 ; skip decrement if already at zero
		dex				; 2 
		stx VB_TIMER	; 4
	@skip_vb_timer:

	update_cursor:
		lda _colorCyclingEnable
		beq return 

		lda RTCLOK_LSB		; update every 16 frames
		tax 
		and #$0F
		bne return 

	cycle_color:
		lda PCOLR0 			; mask out luminance
		and #$F0
		sta PCOLR0 

		txa 
		bpl @skip_eor
			eor #$FF
		@skip_eor:
		lsr a
		lsr a
		lsr a
		ora PCOLR0 

		sta PCOLR0			; store new color value in players 0 and 1
		sta PCOLR1

	return:
		jmp XITVBV
.endproc


.proc _mapViewDLI
	pha					; push accumulator and X register onto stack
	txa
	pha
	
	lda VCOUNT 			; tile DLI: 21, 29, 37 ...

	lastTileLine = vcountTopMargin + 8 * 9

	cmp #lastTileLine
	bcc reposition_sprite

	sta WSYNC			; wait for horizontal sync
	cmp #lastTileLine+7
	bcc chara_name 
	
	cmp #lastTileLine+11
	bcc chara_level
	
	cmp #lastTileLine+19
	bcc chara_hp
	
	cmp #lastTileLine+23
	bcc party_stats

	jmp last_line

	reposition_sprite:
		sec 
		sbc #vcountTopMargin ; X = (VCOUNT - topMargin) / 8
		lsr a 
		lsr a 
		lsr a 
		tax 
		lda _dliSpriteData,X; HPOSP3 = _dliSpriteData[X]
		sta WSYNC			; wait for horizontal sync
		sta HPOSP3
		jmp return_dli

	chara_name:
		lda #0
		sta COLPF4			; border
		sta COLPF2			; text box background color
		lda TXTLUM		
		sta COLPF1			; text color
		jmp return_dli

	chara_level:
		lda PCOLR2		
		sta COLPF1			; text color
		jmp return_dli

	chara_hp:
		lda TXTLUM		
		sta COLPF1			; text color
		jmp return_dli

	party_stats:
		lda TXTBKG			; border
		sta COLPF2			; text box background color
		jmp return_dli

	last_line:
		lda COLOR2 			; restore colors for map in case VBI is missed
		sta COLPF2
		lda _dliSpriteData 	; set P3 sprite position so first row is correct
		sta HPOSP3

	return_dli:	
		pla					; restore accumulator and X register from stack
		tax
		pla
		rti
.endproc

.proc _battleViewDLI
	pha					; push accumulator and Y register onto stack
	tya
	pha
	
	lda VCOUNT 			; use debugger to get actual VCOUNT values

	vcountOffset = $39 	; 57
	cmp #vcountOffset
		bcc colors 
		beq enemy_hp 			; VCOUNT = $39

	cmp #vcountOffset+4*7
		bcc chara_name			; VCOUNT = $52 or 82

	cmp #vcountOffset+4*9
		bcc chara_level			; VCOUNT = $59

	cmp #vcountOffset+4*11
		bcc chara_hp			; VCOUNT = $61

	cmp #vcountOffset+4*12
		bcc button_bar			; VCOUNT = $65

	jmp last_line 

	colors: 
		ldy dliColorTable+1  ; check for NULL pointer
		beq return_dli

		sec 
		sbc #vcountTopMargin
		tay 
		sta WSYNC			; wait for horizontal sync
		lda (dliColorTable),Y
		sta COLPF4
		jmp return_dli

	enemy_hp:
		lda TXTBKG
			sta WSYNC			; wait for horizontal sync
			sta COLPF2			; dialog box background
		lda TXTLUM
			sta COLPF1			; text luminance / bar chart foreground
		lda #$00	
			sta COLPF4			; border background: black
		lda COLOR7
			sta COLPF0			; bar chart background color: blue
		jmp return_dli

	chara_name:
		lda #0
		sta COLPF4			; border
		sta COLPF2			; text box background color
		lda TXTLUM		
		sta COLPF1			; text color
		jmp return_dli

	chara_level:
		lda PCOLR2		
		sta COLPF1			; text color
		jmp return_dli

	chara_hp:
		lda TXTLUM		
		sta COLPF1			; bar color
		jmp return_dli

	button_bar:
		lda COLOR0		
		ldy COLOR1		
		sta COLPF0
		sty COLPF1
		lda COLOR2			
		sta COLPF2
		jmp return_dli

	last_line: 
		sta WSYNC			; wait for horizontal sync
		lda COLOR4 
		sta COLPF4

	return_dli:	
		pla					; restore accumulator and Y register from stack
		tay
		pla
		rti
.endproc

.proc _infoViewDLI
	pha					; push AX onto stack
	txa 
	pha 

	lda VCOUNT 			; use debugger to get actual VCOUNT values

	cmp #vcountTopMargin + 96
	bcs last_line

	cmp #vcountTopMargin + 24
	bcc reposition_sprites
	beq title_rows

	cmp #vcountTopMargin + 77
	beq title_rows

	jmp normal_rows

	title_rows:
		lda TXTLUM 		 	; swap background and text color	
		ldx #0
		sta WSYNC			; wait for horizontal sync
		sta COLPF2			; text box background
		stx COLPF1			; text luminance / bar chart foreground
		jmp return

	normal_rows:
		lda TXTBKG
		ldx TXTLUM
		sta WSYNC			; wait for horizontal sync
		sta COLPF2			; text box background
		stx COLPF1			; text luminance / bar chart foreground
		jmp return

	reposition_sprites:
		lda COLOR4			; set missiles to the same color as background
		sta COLPF3 
		lda _dliSpriteData
		sta HPOSM0
		lda _dliSpriteData+1
		sta HPOSM1
		lda _dliSpriteData+2
		sta HPOSM2
		lda _dliSpriteData+3
		sta HPOSM3
		jmp return

	last_line:
		lda #0
		sta HPOSM0
		sta HPOSM1
		sta HPOSM2
		sta HPOSM3

	return:
		pla 
		tax 	
		pla
		rti
.endproc
