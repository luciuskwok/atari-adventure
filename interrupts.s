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

.proc _initVBI
			; MSB is in X register
	tay		; LSB is in A register, so transfer to Y
	lda #6		; 6=Immediate, 7=Deferred.
	jsr $E45C	;SETVBV
	rts
.endproc	
	
.proc _immediateUserVBI
cursor_timer = $0600		; timer is used to slow down the updates
cursor_target = $0601
cursor_color = $0602
selection_color = $0603
joystick_timer = $0610		; countdown timer for repeating joystick moves

	lda joystick_timer	; sets Z flag (Z=1 if joystick_timer is zero)
	beq update_cursor	; skip decrement if already at zero
	dec joystick_timer
	
update_cursor:
	lda cursor_timer	
	sbc #1
	sta cursor_timer	; store updated timer value
	bpl return		; if timer >= 0, skip updating the color
	lda #6			; reset the timer to 5 ticks
	sta cursor_timer
				
				; == Cycle only the luminance of the color value ==
	lda PCOLR0		
	and #$0F		; isolate luminance in A register
	cmp cursor_target
	bne update_color

				; == Reset the target if the color is equal to target
	cmp #$08		; if A is < $08, set cursor_target to $0F
	bcc reset_target_to_max
reset_target_to_zero:
	ldy #$00		; reset target to #00
	sty cursor_target
	clv
	bvc update_color
reset_target_to_max:
	ldy #$0D		; reset target to $0D
	sty cursor_target
	
update_color:
	cmp cursor_target
	bcc increment_luminance
decrement_luminance:
	sbc #1			; decrement luminance
	clv
	bvc write_color
increment_luminance:
	adc #1
	
write_color:
	ora cursor_color	; add color hue for cursor
	sta PCOLR0
	and #$0F		; replace color hue for selection
	ora selection_color
	sta COLOR2
	
	
return:
	jmp $E45F		; jump to the OS immediate VBI routine
.endproc