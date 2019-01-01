; interrupts.s
;
; void __fastcall__ initVBI(void *addr);
; void __fastcall__ immediateUserVBI(void);
;

.export _initVBI
.export _immediateUserVBI
.export _mapViewDLI
.export _battleViewDLI
.export _infoViewDLI

.import _soundState

.code
; Constants
	PCOLR0 = $02C0		; Player 0 color
	PCOLR1 = $02C1		; Player 1 color
	COLOR0 = $02C4		; Field 0 color (pixel value 1)
	COLOR1 = $02C5		; Field 1 color (pixel value 2)
	COLOR2 = $02C6		; Field 2 color (pixel value 3)
	COLOR3 = $02C7		; Field 3 color
	COLOR4 = $02C8		; Background color (pixel value 0)
	TXTLUM = $02C9		; Extra color: Text luminance
	TXTBKG = $02CA		; Extra color: Text box background color
	COLOR7 = $02CB		; Extra color: Bar background color

	; GTIA
	HPOSP3 = $D003		; Player 2 horizontal position
	COLPF0 = $D016		; pixel 1
	COLPF1 = $D017		; text luminance / pixel 2
	COLPF2 = $D018		; text background / pixel 3
	COLPF3 = $D019		; 
	COLPF4 = $D01A		; background / pixel 0

	; POKEY 
	AUDF1 = $D200
	AUDC1 = $D201
	AUDF2 = $D202
	AUDC2 = $D203
	AUDF3 = $D204
	AUDC3 = $D205
	AUDF4 = $D206
	AUDC4 = $D207

	; ANTIC
	CHBASE = $D409		; Character set
	WSYNC  = $D40A
	VCOUNT = $D40B		; vertical line counter

	CUR_SKIP  = 15		; number of frames to skip for color cycling
	CUR_TIMER = $0600	; cursor color cycling frame skip countdown timer
	VB_TIMER  = $0601	; general countdown timer that decrements every VBI
	DLI_ROW   = $0602	; for keeping track of which row the DLI is on. Easier to use this for iterating through P3_XPOS and BG_COLOR arrays than to use VCOUNT.
	P3_XPOS   = $0603	; array of 9 bytes for repositioning player 3

	VIBRATO_TIMER = _soundState	; timer for vibrato effect 
	CHANNEL_STATE = _soundState+1
	CHANNEL_SIZE = 9
	CH_FREQ     = 0			; frequency divisor as AUDF value
	CH_VIBR     = 1 		; duty cycle for vibrato
	CH_CUR_LVL  = 2		; current volume level
	CH_ATK_TIME = 3
	CH_ATK_RATE = 4
	CH_DEC_RATE = 5
	CH_SUS_LVL  = 6
	CH_SUS_TIME = 7
	CH_REL_TIME = 8

; End Constants


.proc _initVBI		; on entry: X=MSB, A=LSB
	tay				; move LSB to Y
	lda #CUR_SKIP	; init CUR_TIMER to CUR_SKIP
	sta CUR_TIMER	
	lda #0
	sta VIBRATO_TIMER
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
	bmi sound 			; if cur_timer > 127, leave the timer alone
	dex 				; else --cur_timer
	stx CUR_TIMER
	bne sound 			; if cur_timer == 0, cycle the colors

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

; ===== Sound Section =====

sound:
	ldy #0
	jsr _applyEnvelopeAtY

sound_vibrato:
	ldy #0
	jsr _getAudFreqCtrlAtY
	stx AUDF1
	sta AUDC1

	ldy #CHANNEL_SIZE*1
	jsr _getAudFreqCtrlAtY
	stx AUDF2
	sta AUDC2

	ldy #CHANNEL_SIZE*2
	jsr _getAudFreqCtrlAtY
	stx AUDF3
	sta AUDC3

	ldy #CHANNEL_SIZE*3
	jsr _getAudFreqCtrlAtY
	stx AUDF4
	sta AUDC4


inc_snd_timer:
	lda VIBRATO_TIMER
	clc
	adc #1
	and #$07 			; keep SND_TIMER in range from 0 to 7
	sta VIBRATO_TIMER

return:
	jmp $E45F			; jump to the OS immediate VBI routine
.endproc


.proc _applyEnvelopeAtY					; $6B06
	lda CHANNEL_STATE+CH_ATK_TIME,Y 	; if atk_time != 0: apply attack
	beq decay

attack:
	sec
	sbc #1
	sta CHANNEL_STATE+CH_ATK_TIME,Y		; atk_time -= 1

	clc
	lda CHANNEL_STATE+CH_CUR_LVL,Y 		; cur_lvl += atk_rate
	adc CHANNEL_STATE+CH_ATK_RATE,Y
	cmp #16								; if cur_lvl >= 16: cur_lvl = 15
	bcc attack_set_lvl
	lda #15
attack_set_lvl:
	sta CHANNEL_STATE+CH_CUR_LVL,Y
	rts 								; return

decay:
	lda CHANNEL_STATE+CH_SUS_TIME,Y		; if sus_time != 0: wait for sustain
	beq release

	lda CHANNEL_STATE+CH_CUR_LVL,Y		; if cur_lvl != sus_lvl: apply decay
	cmp CHANNEL_STATE+CH_SUS_LVL,Y
	beq sustain

	sec
	sbc CHANNEL_STATE+CH_DEC_RATE,Y 	; cur_lvl -= dec_rate

	cmp CHANNEL_STATE+CH_SUS_LVL,Y		; if cur_lvl < sus_lvl: cur_lvl = sus_lvl
	bcs set_cur_level
	lda CHANNEL_STATE+CH_SUS_LVL,Y

set_cur_level:
	sta CHANNEL_STATE+CH_CUR_LVL,Y
	rts 								; return

sustain:
	lda CHANNEL_STATE+CH_SUS_TIME,Y		; if sus_time != 0: wait for sustain
	beq release

	sec
	sbc #1								; sus_time -= 1
	sta CHANNEL_STATE+CH_SUS_TIME,Y
	rts									; return

release:
	lda CHANNEL_STATE+CH_CUR_LVL,Y 		; if cur_lvl != 0: apply release
	beq return	
	sec
	sbc #1
	sta CHANNEL_STATE+CH_CUR_LVL,Y

return:
	rts
.endproc


.proc _getAudFreqCtrlAtY
	; on input: Y=offset for channel
	; on output: X=AUDF value, A=AUDC value
	lda VIBRATO_TIMER
	cmp CHANNEL_STATE+CH_VIBR,Y ; if vibrato_timer >= ch_vibr, set carry
	lda CHANNEL_STATE+CH_FREQ,Y
	tax
	bcs audctrl
	inx
audctrl:
	lda CHANNEL_STATE+CH_CUR_LVL,Y
	and #$0F
	ora #$E0
	rts
.endproc

.proc _mapViewDLI
	pha					; push accumulator and X register onto stack
	txa
	pha
	
	ldx DLI_ROW			; increment DLI_ROW
	inx
	stx DLI_ROW
	stx WSYNC			; wait for horizontal sync

	cpx #9				; row 9: upper text window
	beq upper_text_window
	
	cpx #10				; row 10: lower text window
	beq lower_text_window
	
	lda P3_XPOS,X		; HPOSP2 = P3_XPOS[DLI_ROW]
	sta HPOSP3
	jmp return_dli
	
upper_text_window:
	lda #$00	
	sta COLPF2			; upper text window is always black
	lda TXTLUM		
	sta COLPF1			; text luminance / bar chart foreground
	jmp return_dli

lower_text_window:
	lda TXTBKG			; lower text window is gray
	sta COLPF2

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
	lda TXTLUM		
	sta COLPF1			; text luminance / bar chart foreground
	lda #$00
	sta COLPF4			; border background: black
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
	pha					; push accumulator and X register onto stack
	
	lda DLI_ROW			; increment DLI_ROW
	clc
	adc #1
	sta DLI_ROW
	sta WSYNC			; wait for horizontal sync

	and #1
	beq even_rows
odd_rows:
	lda TXTLUM 		 	; swap background and text color	
	sta COLPF2			; text box background
	lda #0
	sta COLPF1			; text luminance / bar chart foreground
	jmp return

even_rows:
	lda TXTBKG
	sta COLPF2			; text box background
	lda TXTLUM
	sta COLPF1			; text luminance / bar chart foreground

return:	
	pla					; restore accumulator and X register from stack
	rti
.endproc
