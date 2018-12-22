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

CUR_SKIP  = 15		; number of frames to skip for color cycling
CUR_TIMER = $0600	; Cursor color cycling frame skip countdown timer
STICK_TIMER = $0601	; Joystick countdown timer for repeating joystick moves
DLI_ROW   = $0602		; for keeping track of which scanline the DLI is on
TEXT_LUM  = $0603	; text luminance for text window
TEXT_BG   = $0604	; text window background color
P3_XPOS   = $0610		; array of 9 bytes for repositioning player 3
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
	lda P3_XPOS			; HPOSP2 = P3_XPOS[0]
	sta HPOSP3

	ldx STICK_TIMER		; sets Z flag (Z=1 if joystick_timer is zero)
	beq update_cursor	; skip decrement if already at zero
	dex
	stx STICK_TIMER
	
update_cursor:
	ldx CUR_TIMER
	bmi return 			; if CUR_TIMER > 127, leave the timer alone
	dex 				; else --CUR_TIMER
	stx CUR_TIMER
	bne return 			; if CUR_TIMER == 0, cycle the colors

cycle_color:
	clv					; clear flags for unconditonal branch
	lda #CUR_SKIP
	sta CUR_TIMER		; reload the timer
						; == Cycle only the luminance of the color value ==						
	lda PCOLR0			; get the color (Addr=3459)
	tax					; use X register for new color value
	and #$0F			; isolate the luminance in A
	cmp #$01			; if luminance is 1
	bne skip_1
	dex					; set luminance to 0
	bvc write_color		; unconditional branch 
skip_1:						
	cmp #$0E			; if luminance is E
	bne skip_2
	inx					; set luminance to F
	bvc write_color		; unconditional branch 
skip_2:
	and #$01			; odd or even
	beq skip_3
	dex 				; if odd, subtract 2
	dex
	bvc write_color		; unconditional branch 
skip_3:
	inx					; else if even, add 2
	inx
write_color:
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
	lda TEXT_LUM		; text window text luminance
	sta COLPF1
	lda TEXT_BG			; text window background color
	sta COLPF2

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
	stx WSYNC			; wait for horizontal sync
	sta COLPF4

	inx					; ++DLI_ROW

	cpx #72		
	bcs return_dli		; if DLI_ROW >= 72, set colors for text window
	
	lda TEXT_LUM		; text window text luminance
	sta COLPF1
	lda TEXT_BG			; text window background color
	sta COLPF2

return_dli:	
	pla					; restore accumulator and X register from stack
	tax
	pla
	rti
.endproc
