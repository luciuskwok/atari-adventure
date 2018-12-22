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
COLPF4 = $D01A		; background
WSYNC  = $D40A

CUR_SKIP = 15		; number of frames to skip for color cycling
CUR_TIMER = $0600	; Cursor color cycling frame skip countdown timer
STICK_TIMER = $0601	; Joystick countdown timer for repeating joystick moves
DLI_ROW = $0602		; for keeping track of which scanline the DLI is on
TEXT_LUM = $0603	; text luminance for text window
TEXT_BG  = $0604	; text window background color
P2_XPOS = $0610		; array of 9 bytes for repositioning player 2
BG_COLOR = $0620	; array of 10 bytes for changing background color, though first byte is ignored

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
	lda CUR_TIMER
	cmp #$00		
	beq cycle_color		; if CUR_TIMER == 0, cycle the colors

	cmp #$FF			
	beq return 			; else if CUR_TIMER == $FF, leave the timer alone

	sec 				; else --CUR_TIMER
	sbc #1
	sta CUR_TIMER
	bcs return

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

	lda BG_COLOR,X 		; set background color for each row
	sta COLPF4

	cpx #9				; if DLI_ROW >= 9, skip to setting text color
	bcs set_text_color
	
	lda P2_XPOS,X		; HPOSP2 = P2_XPOS[DLI_ROW]
	sta HPOSP2
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