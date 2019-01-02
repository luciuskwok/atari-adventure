; interrupts.s

.export _initVBI			; void __fastcall__ initVBI(void *addr);
.export _immediateUserVBI
.export _mapViewDLI
.export _battleViewDLI
.export _infoViewDLI

.export _testTriggerNote	; void __fastcall__ testTriggerNote(UInt8 note);

.import _soundState
.import _noteTable

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

	vibratoTimer = _soundState	; timer for vibrato effect 
	seqTimer   = _soundState+1  ; timer for sequencer step
	seqStepDur = _soundState+2 ; number of ticks per sequencer step

	chState    = _soundState+3
	chSize     = 13
	chFreq     = 0			; frequency divisor as AUDF value
	chVibr     = 1 			; duty cycle for vibrato
	chCurLvl   = 2			; current volume level
	chAtkTime  = 3
	chAtkRate  = 4
	chDecRate  = 5
	chSusLvl   = 6
	chSusTime  = 7
	chRelRate  = 8

	seqIndex   = 9
	noteStepsLeft = 10
	seqBlockPtr= 11

; Zero Page usage (SOUND_AREA)
	ptrSeqBlock = $AC 		; pointer to SequencerBlock
	tmpIndex	= $AE
	chStateOffset = $AF 

; End Constants


.proc _initVBI		; on entry: X=MSB, A=LSB
	tay				; move LSB to Y
	lda #CUR_SKIP	; init CUR_TIMER to CUR_SKIP
	sta CUR_TIMER	
	lda #0
	sta vibratoTimer
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
	lda seqStepDur
	beq apply_envelopes 		; if seqStepDur == 0: sequencer is disabled

	ldx seqTimer				; seqTimer -= 1
	dex
	beq execute_seq_step 		; if seqTimer == 0: execute sequencer step
	stx seqTimer				; else store seqTimer
	jmp apply_envelopes

execute_seq_step:
	sta seqTimer 				; reset seqTimer = seqStepDur
	ldx #0
	jsr _stepSequenceAtX

apply_envelopes:
	ldy #0						; channel 0
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF1
	sta AUDC1

	ldy #chSize*1				; channel 1
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF2
	sta AUDC2

	ldy #chSize*2				; channel 2
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF3
	sta AUDC3

	ldy #chSize*3				; channel 3
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF4
	sta AUDC4

inc_vibrato_timer:
	lda vibratoTimer 			; vibratoTimer += 1
	clc
	adc #1
	and #$07 					; keep vibratoTimer in range from 0 to 7
	sta vibratoTimer

return:
	jmp $E45F					; jump to the OS immediate VBI routine
.endproc


.proc _stepSequenceAtX
	; on entry: X=offset to channel state
	; Y is not preserved on return
	lda chState+noteStepsLeft,X ; if noteStepsLeft == 0: execute next step in sequence
	beq next_step
	sec 
	sbc #1
	sta chState+noteStepsLeft,X ; noteStepsLeft -= 1
	rts			

next_step:
	lda chState+seqBlockPtr,X	; ptrSeqBlock = chState[X].seqBlockPtr
	sta ptrSeqBlock
	lda chState+seqBlockPtr+1,X
	sta ptrSeqBlock+1
	beq return				 	; check for nil ptr

	lda chState+seqIndex,X 		; offset = seqIndex * 4 + 1
	asl a						; count is 1 byte
	asl a						; sequencer note unit is 4 bytes
	clc
	adc #1
	pha 						; temporarily store offset on stack

	tay
	lda (ptrSeqBlock),Y 		; ptrSeqBlock->note[seqIndex]->noteNumber
	jsr _setNoteAtX 				; uses Y, so offset needed to be stored

	pla 						; pull offset off stack
	tay
	iny
	lda (ptrSeqBlock),Y 		; ptrSeqBlock->note[seqIndex]->duration
	sta chState+noteStepsLeft,X
	sta chState+chSusTime,X

	iny
	lda (ptrSeqBlock),Y 		; ptrSeqBlock->note[seqIndex]->volume
	sta chState+chSusLvl,X

	iny
	lda (ptrSeqBlock),Y 		; ptrSeqBlock->note[seqIndex]->envelope
	jsr _setEnvelopeAtX

inc_seq_index:
	lda chState+seqIndex,X		; seqIndex += 1
	clc 
	adc #1
	ldy #0
	cmp (ptrSeqBlock),Y 		; if seqIndex == ptrSeqBlock->count:
	bne set_seq_index
	lda #0						; seqIndex = 0; repeat sequence block
set_seq_index:
	sta chState+seqIndex,X

return:
	rts
.endproc

.proc _testTriggerNote 			; $6C39
	; on entry: A=note number
	ldx #chSize*3 				; $926D
	jsr _setNoteAtX

	lda #10
	sta chState+chSusTime,X

	lda #8
	sta chState+chSusLvl,X

	lda #1
	jsr _setEnvelopeAtX
	rts
.endproc

.proc _setNoteAtX
	; on entry: A=note number
	; uses Y register
	asl a
	tay
	lda _noteTable,Y			; freq = noteTable[Y].audf
	sta chState+chFreq,X
	lda _noteTable+1,Y			; vibr = noteTable[Y].vibrato
	sta chState+chVibr,X
	rts
.endproc

.proc _setEnvelopeAtX
	cmp #1
	beq envelope_1
	cmp #2
	beq envelope_2
	jmp default_envelope

envelope_1:
	lda #8
	sta chState+chAtkRate,X 	; fast attack

	lda #2
	sta chState+chAtkTime,X

	lda #1
	sta chState+chDecRate,X 	; slow decay and release
	sta chState+chRelRate,X

	lda #0
	sta chState+chCurLvl,X 		; start at currentLevel = 0

	lda chState+chSusTime,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	sec
	sbc #$11					; and subtract time for atk, dec,& rel
	bcs set_sustain_time 		; if sus_time < 0: sus_time = 0
	lda #0
	jmp set_sustain_time

envelope_2:
	lda #15
	sta chState+chAtkTime,X		; slow attack

	lda #1
	sta chState+chAtkRate,X
	sta chState+chDecRate,X 	; slow decay and release
	sta chState+chRelRate,X

	lda #0
	sta chState+chCurLvl,X 		; start at currentLevel = 0

	lda chState+chSusTime,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	sec
	sbc #$1E					; and subtract time for atk, dec,& rel
	bcs set_sustain_time 		; if sus_time < 0: sus_time = 0
	lda #0
	jmp set_sustain_time

default_envelope:
	lda #0
	sta chState+chAtkTime,X 	; no attack
	sta chState+chAtkRate,X

	lda #15
	sta chState+chDecRate,X 	; instant decay & release
	sta chState+chRelRate,X

	lda chState+chSusLvl,X		; start at currentLevel = sustainLevel
	sta chState+chCurLvl,X

	lda chState+chSusTime,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	;jmp set_sustain_time

set_sustain_time:
	sta chState+chSusTime,X
return:
	rts
.endproc

.proc _multiplyByStepDuration
	; A = A * step duration
	tay
	lda #0
	jmp while
loop:
	clc
	adc seqStepDur
	dey
while:
	cpy #0
	bne loop
	rts
.endproc

.proc _applyEnvelopeAtY			; $6B06
	; on entry: Y=offset to channel state
	; Y is preserved on return
	lda chState+chCurLvl,Y 	; use X for cur_lvl
	tax
	lda chState+chAtkTime,Y 	; if atk_time != 0: apply attack
	beq decay

attack:
	sec
	sbc #1
	sta chState+chAtkTime,Y	; atk_time -= 1

	clc
	txa
	adc chState+chAtkRate,Y	; cur_lvl += atk_rate
	sta chState+chCurLvl,Y
	rts 						; return

decay:
	txa
	cmp chState+chSusLvl,Y	; if cur_lvl > sus_lvl: apply decay
	beq sustain
	bcc sustain

	sec
	sbc chState+chDecRate,Y 	; cur_lvl -= dec_rate
	sta chState+chCurLvl,Y
	rts 						; return

sustain:
	lda chState+chSusTime,Y	; if sus_time != 0: wait for sustain
	beq release

	sec
	sbc #1						; sus_time -= 1
	sta chState+chSusTime,Y
	rts							; return

release:
	cpx #0 						; if cur_lvl != 0: apply release
	beq return
	txa	
	sec
	sbc chState+chRelRate,Y
	bcs set_release_level
	lda #0
set_release_level:
	sta chState+chCurLvl,Y

return:
	rts
.endproc


.proc _getAudFreqCtrlAtY
	; on entry: Y=offset to channel state
	; Y is preserved on return
	; on return: X=AUDF value, A=AUDC value
	lda vibratoTimer
	cmp chState+chVibr,Y ; if vibratoTimer >= chVibr, set carry
	lda chState+chFreq,Y
	tax
	bcs audctrl
	inx
audctrl:
	lda chState+chCurLvl,Y
	cmp #$10
	bcc below_limit
	lda #$0F
below_limit:
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
