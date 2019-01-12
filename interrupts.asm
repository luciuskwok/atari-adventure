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

	CUR_SKIP  = 15		; number of frames to skip for color cycling
	CUR_TIMER = $0600	; cursor color cycling frame skip countdown timer
	VB_TIMER  = $0601	; general countdown timer that decrements every VBI
	DLI_ROW   = $0602	; for keeping track of which row the DLI is on. Could also use VCOUNT.

; End Constants

.code

.proc _initVBI		; 
	lda #CUR_SKIP	; init CUR_TIMER to CUR_SKIP
	sta CUR_TIMER	

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
	

.proc _immediateUserVBI
	lda #0				; reset DLI_ROW
	sta DLI_ROW
	jsr _soundVBI
	jmp SYSVBV			; jump to the OS immediate VBI routine
.endproc


.proc _deferredUserVBI	
	ldx VB_TIMER		; 4 ; sets Z flag if zero
	beq update_sound_sprites	; 2 ; skip decrement if already at zero
	dex					; 2 
	stx VB_TIMER		; 4

	update_sound_sprites:
		jsr _updateSoundSprites

	update_cursor:
		ldx CUR_TIMER
		bmi return 			; if cur_timer > 127, leave the timer alone
		dex 				; else --cur_timer
		stx CUR_TIMER
		bne return 			; if cur_timer == 0, cycle the colors

	cycle_color:
		lda #CUR_SKIP		; reset cur_timer to cur_skip
		sta CUR_TIMER		; 
		
		; == Cycle luminance of the player ==	
		; if color is even, add 2 
		; if color is odd, subtract 2
		; 0 2 4 6 8 A C E . F D B 9 7 5 3 1

		lda PCOLR0			; 
		tax					; copy color -> X
		and #$0F			; isolate the lum in A
		lsr a				; if lum is odd: set carry

		bcc color_is_even  	
		dex					; lum is odd: X -= 1
		cmp #$01 			; if lum != 1, X -= 1 again
		beq set_player_color
		dex
		jmp set_player_color

	color_is_even:
		inx					; lum is even: X += 1
		cmp #$07			; if lum != 7, X += 1 again
		beq set_player_color
		inx 

	set_player_color:
		stx PCOLR0			; store new color value in players 0 and 1
		stx PCOLR1

	return:
		jmp XITVBV
.endproc


.proc _mapViewDLI
	pha					; push accumulator and X register onto stack
	txa
	pha
	
	ldx DLI_ROW			; increment DLI_ROW
	inx
	stx DLI_ROW

	cpx #9				; before line 9
	bcc reposition_sprite

	stx WSYNC			; wait for horizontal sync
	beq chara_name 		; line 9
	
	cpx #10				; 
	beq chara_level
	
	cpx #11				; 
	beq chara_hp
	
	cpx #12				; 
	beq party_stats
	
	cpx #13				; 
	beq last_line

	jmp return_dli

	reposition_sprite:
		lda _dliSpriteData,X; HPOSP3 = _dliSpriteData[DLI_ROW]
		stx WSYNC			; wait for horizontal sync
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
	pha					; push accumulator and X register onto stack
	txa
	pha
	
	ldx DLI_ROW			; increment DLI_ROW
	inx
	stx DLI_ROW
	stx WSYNC			; wait for horizontal sync

	cpx #2
	beq button_bar

	text_window:
		lda #$00	
		sta COLPF2			; text box background: black
		sta COLPF4			; border background: black
		lda TXTLUM		
		sta COLPF1			; text luminance / bar chart foreground
		lda COLOR7
		sta COLPF0			; bar chart background color: blue
		jmp return_dli

	button_bar:
		lda COLOR0			; reload shadow register values
		sta COLPF0
		lda COLOR1			; reload shadow register values
		sta COLPF1
		lda COLOR2			
		sta COLPF2

	return_dli:	
		pla					; restore accumulator and X register from stack
		tax
		pla
		rti
.endproc

.proc _infoViewDLI
	pha					; push AX onto stack
	txa 
	pha 
	ldx DLI_ROW			; increment DLI_ROW
	inx
	stx DLI_ROW

	cpx #6
	beq last_line

	cpx #1
	beq reposition_sprites

	txa
	and #1
	bne normal_rows

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
