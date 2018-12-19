; interrupts.s
;
; void __fastcall__ initVBI(void *addr);
; void __fastcall__ immediateUserVBI(void);
;

.export _initVBI
.export _immediateUserVBI
.code

PCOLR0 = $02C0		; Player 0 color
PCOLR1 = $02C1		; Player 1 color
CUR_SKIP = 10		; number of frames to skip for color cycling
CUR_TIMER = $0600	; Cursor color cycling frame skip countdown timer
STICK_TIMER = $0610	; Joystick countdown timer for repeating joystick moves

.proc _initVBI		; on entry: X=MSB, A=LSB
	tay				; move LSB to Y
	lda #CUR_SKIP	; init CUR_TIMER to CUR_SKIP
	sta CUR_TIMER	
	lda #6			; 6=Immediate, 7=Deferred.
	jsr $E45C		; SETVBV
	rts
.endproc
	
.proc _immediateUserVBI

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