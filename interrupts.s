; interrupts.s
;
; void __fastcall__ initVBI(void *addr);
; void __fastcall__ immediateUserVBI(void);
;

.export _initVBI
.export _immediateUserVBI
.export _displayListInterrupt

.code
PCOLR0 = $02C0		; Player 0 color
PCOLR1 = $02C1		; Player 1 color

CHBASE = $D409		; Character set
HPOSP2 = $D002		; Player 2 horizontal position
COLPF0 = $D016		; 
COLPF1 = $D017		; text luminance
COLPF2 = $D018		; text background
COLPF3 = $D019		; 
WSYNC  = $D40A

CUR_SKIP = 15		; number of frames to skip for color cycling
CUR_TIMER = $0600	; Cursor color cycling frame skip countdown timer
STICK_TIMER = $0601	; Joystick countdown timer for repeating joystick moves
DLI_ROW = $0603		; for keeping track of which scanline the DLI is on
P2_XPOS = $0604		; array of 11 bytes for repositioning player 2

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
	lda P2_XPOS			; HPOSP2 = P2_XPOS[0]
	sta HPOSP2

	lda STICK_TIMER		; sets Z flag (Z=1 if joystick_timer is zero)
	beq update_cursor	; skip decrement if already at zero
	dec STICK_TIMER
	
update_cursor:
	clv					; clear overflow flag just in case
	dec CUR_TIMER	
	bne return			; if timer != 0, skip updating the color
	lda #CUR_SKIP		; reset the timer to 5 ticks
	sta CUR_TIMER
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

.proc _displayListInterrupt
	pha					; push accumulator and X register onto stack
	txa
	pha
	
	lda DLI_ROW			; Check DLI_ROW
	clc
	adc #1				; ++DLI_ROW
	tax
	sta DLI_ROW
	sta WSYNC			; wait for horizontal sync
	cpx #9				; if DLI_ROW >= 9, skip to setting text color
	bcs text_color
	
	lda P2_XPOS,X		; HPOSP2 = P2_XPOS[DLI_ROW]
	sta HPOSP2
	bvc return_dli
	
text_color:
	lda #$0E			; Set text window to use white on dark gray
	sta COLPF1			; text luminance
	lda #$04
	sta COLPF2			; text background

return_dli:	
	pla					; restore accumulator and X register from stack
	tax
	pla
	rti
.endproc