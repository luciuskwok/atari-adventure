; interrupts.s
;
; void __fastcall__ initVBI(void *addr);
; void __fastcall__ immediateUserVBI(void);
;

.export _initVBI
.export _immediateUserVBI
.export _mapViewDLI
.export _storyViewDLI

.code
PCOLR0 = $02C0		; Player 0 color
PCOLR1 = $02C1		; Player 1 color

CHBASE = $D409		; Character set
HPOSP3 = $D003		; Player 2 horizontal position
COLPF0 = $D016		; 
COLPF1 = $D017		; text luminance
COLPF2 = $D018		; text background
COLPF3 = $D019		; 
COLPF4 = $D01A		; background
WSYNC  = $D40A
VCOUNT = $D40B		; vertical line counter

CUR_SKIP  = 15		; number of frames to skip for color cycling
CUR_TIMER = $0600	; cursor color cycling frame skip countdown timer
VB_TIMER = $0601	; general countdown timer that decrements every VBI
DLI_ROW   = $0610	; for keeping track of which row the DLI is on. Easier to use this for iterating through P3_XPOS and BG_COLOR arrays than to use VCOUNT.
TEXT_LUM  = $0611	; text luminance for text window
TEXT_BG   = $0612	; text window background color
P3_XPOS   = $0613	; array of 9 bytes for repositioning player 3
BG_COLOR  = $0620	; array of 72 bytes for changing background color per raster line


.proc _initVBI		; on entry: X=MSB, A=LSB
	tay				; move LSB to Y
	lda #CUR_SKIP	; init CUR_TIMER to CUR_SKIP
	sta CUR_TIMER	
	lda #6			; 6=Immediate, 7=Deferred.
	jsr $E45C		; SETVBV
	rts
.endproc
	

.proc _immediateUserVBI
	lda #0				; reset DLI_ROW
	sta DLI_ROW
	lda P3_XPOS			; HPOSP3 = P3_XPOS[0]
	sta HPOSP3

	ldx VB_TIMER		; 4 ; sets Z flag if zero
	beq update_cursor	; 2 ; skip decrement if already at zero
	dex					; 2 
	stx VB_TIMER		; 4
	
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
	jmp $E45F			; jump to the OS immediate VBI routine
.endproc


.proc _mapViewDLI
	pha					; push accumulator and X register onto stack
	txa
	pha
	
	ldx DLI_ROW			; increment DLI_ROW
	inx
	stx DLI_ROW
	stx WSYNC			; wait for horizontal sync

	cpx #9				; if DLI_ROW >= 9, skip to setting text color
	bcs set_text_color
	
	lda P3_XPOS,X		; HPOSP2 = P3_XPOS[DLI_ROW]
	sta HPOSP3
	bvc return_dli
	
set_text_color:
	lda TEXT_BG			; text window background color
	sta COLPF2
	lda TEXT_LUM		; text window text luminance
	sta COLPF1

return_dli:	
	pla					; restore accumulator and X register from stack
	tax
	pla
	rti
.endproc

.proc _storyViewDLI
	pha					; push accumulator and X register onto stack
	txa
	pha
	
	ldx DLI_ROW			 
	lda BG_COLOR,X 		; set background color for each row

	inx					; ++DLI_ROW
	stx DLI_ROW

	sta WSYNC			; wait for horizontal sync
	sta COLPF4

	cpx #72
	bne return_dli		; if DLI_ROW == 72, set colors for text window
	
	lda TEXT_BG			; text window background color
	sta COLPF2
	lda TEXT_LUM		; text window text luminance
	sta COLPF1

return_dli:	
	pla					; restore accumulator and X register from stack
	tax
	pla
	rti
.endproc
