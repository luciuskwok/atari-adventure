; sound.s

; Exports
.export _noteOn		; void __fastcall__ noteOn(UInt8 note);
.export _noteOff	; void __fastcall__ noteOff(void);
.export _startSequence ; void __fastcall__ startSequence(void);
.export _stopSound	; void __fastcall__ stopSound(void);

.export _soundVBI	; called from immediateUserVBI
.export _initSound 	; called from initVBI


; Constants
	; POKEY 
	AUDF1 = $D200
	AUDC1 = $D201
	AUDF2 = $D202
	AUDC2 = $D203
	AUDF3 = $D204
	AUDC3 = $D205
	AUDF4 = $D206
	AUDC4 = $D207
	AUDCTL= $D208
	SKCTL = $D20F


; Zero Page usage (SOUND_AREA)
; TODO: use proper zeropage segment for this
	ptrSeqBlock = $AC 		; pointer to SequencerBlock
	zpUnused1	= $AE
	zpUnused2   = $AF 


; Note and Octave constants
	NoteC  = 0
	NoteDb = 1
	NoteD  = 2
	NoteEb = 3
	NoteE  = 4
	NoteF  = 5
	NoteGb = 6
	NoteG  = 7
	NoteAb = 8
	NoteA  = 9
	NoteBb = 10
	NoteB  = 11
	Oct3   = 0
	Oct4   = 12
	Oct5   = 24
	Oct6   = 36
	Oct7   = 48  ; Only C7 exists. Do not use above C7.

.rodata
noteTable:
	.byte 255, 0 ; C3
	.byte 240, 5 ; C#
	.byte 227, 1 ; D
	.byte 214, 2 ; D#
	.byte 202, 1 ; E
	.byte 190, 6 ; F
	.byte 180, 0 ; F#
	.byte 169, 7 ; G
	.byte 160, 2 ; G#
	.byte 151, 2 ; A
	.byte 142, 5 ; A#
	.byte 134, 5 ; B
	.byte 127, 0 ; C4
	.byte 119, 7 ; C#
	.byte 113, 1 ; D
	.byte 106, 5 ; D#
	.byte 100, 5 ; E
	.byte  94, 7 ; F
	.byte  89, 4 ; F#
	.byte  84, 3 ; G
	.byte  79, 5 ; G#
	.byte  75, 1 ; A
	.byte  70, 7 ; A#
	.byte  66, 6 ; B
	.byte  63, 0 ; C5
	.byte  59, 3 ; C#
	.byte  56, 0 ; D
	.byte  52, 7 ; D#
	.byte  49, 6 ; E
	.byte  47, 0 ; F
	.byte  44, 2 ; F#
	.byte  41, 6 ; G
	.byte  39, 3 ; G#
	.byte  37, 0 ; A
	.byte  34, 7 ; A#
	.byte  32, 7 ; B
	.byte  31, 0 ; C6
	.byte  29, 2 ; C#
	.byte  27, 4 ; D
	.byte  25, 7 ; D#
	.byte  24, 3 ; E
	.byte  23, 0 ; F
	.byte  21, 5 ; F#
	.byte  20, 3 ; G
	.byte  19, 1 ; G#
	.byte  18, 0 ; A
	.byte  17, 0 ; A#
	.byte  16, 0 ; B
	.byte  15, 0 ; C7

testSequence:
	.byte 13 ; note count
	.byte NoteC+Oct4, 4, 2, 1 ; note, duration, volume, envelope
	.byte NoteC+Oct5, 4, 2, 1
	.byte NoteG+Oct4, 8, 2, 1

	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteC+Oct5, 4, 2, 1
	.byte NoteC+Oct4, 8, 2, 1

	.byte NoteC+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteC+Oct5, 4, 2, 1
	.byte NoteD+Oct5, 4, 2, 1

	.byte NoteC+Oct5, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 8, 2, 1


.data
vibratoTimer:
	.byte 0
seqTimer:
	.byte 0
seqStepDur:		; controls tempo
	.byte 9 

chFreq: 		; channel state start
	.byte 0
chVibr: 		; duty cycle for vibrato
	.byte 0
chCurLvl:		; current volume level
	.byte 0
chAtkTime:
	.byte 0
chAtkRate:
	.byte 0
chDecRate:
	.byte 0
chSusLvl:
	.byte 0
chSusTime:
	.byte 0
chRelRate:
	.byte 0
seqEnable:
	.byte 0
seqIndex:
	.byte 0
noteStepsLeft:
	.byte 0
seqBlockPtr:
	.word 0

chState1:
	.res 14*3, $00

chSize = 14 ; bytes per channel state block

.code

.proc _soundVBI

; ===== Sound Section =====

sound:
	lda seqStepDur
	beq apply_envelopes 		; if seqStepDur == 0: sequencer is disabled

	ldx seqTimer				; 
	beq execute_seq_step 		; if seqTimer == 0: execute sequencer step
	dex							; else: seqTimer -= 1
	stx seqTimer
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

	rts
.endproc


.proc _stepSequenceAtX
	; on entry: X=offset to channel state
	; Y is not preserved on return
	lda seqEnable,X 		; if seqEnable == 0: return
	beq return

	lda noteStepsLeft,X		; if noteStepsLeft == 0: execute next step in sequence
	beq next_step

	sec  					; noteStepsLeft -= 1
	sbc #1
	sta noteStepsLeft,X
return:
	rts			

next_step:
	lda seqBlockPtr,X		; ptrSeqBlock = chState[X].seqBlockPtr
	sta ptrSeqBlock
	lda seqBlockPtr+1,X
	sta ptrSeqBlock+1

	lda seqIndex,X 			; offset = seqIndex * 4 + 1
	asl a					; count is 1 byte
	asl a					; sequencer note unit is 4 bytes
	clc
	adc #1
	pha 					; temporarily store offset on stack

	tay
	lda (ptrSeqBlock),Y 	; ptrSeqBlock->note[seqIndex]->noteNumber
	jsr _setNoteAtX 		; uses Y, so offset needed to be stored

	pla 					; pull offset off stack
	tay
	iny
	lda (ptrSeqBlock),Y 	; ptrSeqBlock->note[seqIndex]->duration
	sta noteStepsLeft,X
	sta chSusTime,X

	iny
	lda (ptrSeqBlock),Y 	; ptrSeqBlock->note[seqIndex]->volume
	sta chSusLvl,X

	iny
	lda (ptrSeqBlock),Y 	; ptrSeqBlock->note[seqIndex]->envelope
	jsr _setEnvelopeAtX

inc_seq_index:
	lda seqIndex,X		; seqIndex += 1
	clc 
	adc #1
	ldy #0
	cmp (ptrSeqBlock),Y 		; if seqIndex == ptrSeqBlock->count:
	bne set_seq_index
	lda #0						; seqIndex = 0; repeat sequence block
set_seq_index:
	sta seqIndex,X
	rts
.endproc

.proc _setNoteAtX
	; on entry: A=note number
	; uses Y register
	asl a						; noteTable has 2 bytes per item
	tay
	lda noteTable,Y			; freq = noteTable[Y].audf
	sta chFreq,X
	lda noteTable+1,Y			; vibr = noteTable[Y].vibrato
	sta chVibr,X
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
	sta chAtkRate,X 	; fast attack

	lda #2
	sta chAtkTime,X

	lda #1
	sta chDecRate,X 	; slow decay and release
	sta chRelRate,X

	lda #0
	sta chCurLvl,X 		; start at currentLevel = 0

	lda chSusTime,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	sec
	sbc #$11					; and subtract time for atk, dec,& rel
	bcs set_sustain_time 		; if sus_time < 0: sus_time = 0
	lda #0
	jmp set_sustain_time

envelope_2:
	lda #15
	sta chAtkTime,X		; slow attack

	lda #1
	sta chAtkRate,X
	sta chDecRate,X 	; slow decay and release
	sta chRelRate,X

	lda #0
	sta chCurLvl,X 		; start at currentLevel = 0

	lda chSusTime,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	sec
	sbc #$1E					; and subtract time for atk, dec,& rel
	bcs set_sustain_time 		; if sus_time < 0: sus_time = 0
	lda #0
	jmp set_sustain_time

default_envelope:
	lda #0
	sta chAtkTime,X 	; no attack
	sta chAtkRate,X

	lda #15
	sta chDecRate,X 	; instant decay & release
	sta chRelRate,X

	lda chSusLvl,X		; start at currentLevel = sustainLevel
	sta chCurLvl,X

	lda chSusTime,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	;jmp set_sustain_time

set_sustain_time:
	sta chSusTime,X
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
	lda chCurLvl,Y 	; use X for cur_lvl
	tax
	lda chAtkTime,Y 	; if atk_time != 0: apply attack
	beq decay

attack:
	sec
	sbc #1
	sta chAtkTime,Y	; atk_time -= 1

	clc
	txa
	adc chAtkRate,Y	; cur_lvl += atk_rate
	sta chCurLvl,Y
	rts 						; return

decay:
	txa
	cmp chSusLvl,Y	; if cur_lvl > sus_lvl: apply decay
	beq sustain
	bcc sustain

	sec
	sbc chDecRate,Y 	; cur_lvl -= dec_rate
	sta chCurLvl,Y
	rts 						; return

sustain:
	lda chSusTime,Y	; if sus_time != 0: wait for sustain
	beq release

	sec
	sbc #1						; sus_time -= 1
	sta chSusTime,Y
	rts							; return

release:
	cpx #0 						; if cur_lvl != 0: apply release
	beq return
	txa	
	sec
	sbc chRelRate,Y
	bcs set_release_level
	lda #0
set_release_level:
	sta chCurLvl,Y

return:
	rts
.endproc


.proc _getAudFreqCtrlAtY
	; on entry: Y=offset to channel state
	; Y is preserved on return
	; on return: X=AUDF value, A=AUDC value
	lda vibratoTimer
	cmp chVibr,Y ; if vibratoTimer >= chVibr, set carry
	lda chFreq,Y
	tax
	bcs audctrl
	inx
audctrl:
	lda chCurLvl,Y
	cmp #$10
	bcc below_limit
	lda #$0F
below_limit:
	and #$0F
	ora #$E0
	rts
.endproc


.proc _noteOn		 			
	; on entry: A=note number
	ldx #chSize*3 				
	jsr _setNoteAtX

	lda #10
	sta chSusTime,X

	lda #8
	sta chSusLvl,X

	lda #1
	jsr _setEnvelopeAtX
	rts
.endproc


.proc _noteOff
	ldx #chSize*3
	lda #0
	sta chSusTime,X
	sta chAtkTime,X
	rts
.endproc


.proc _startSequence
	ldx #0
	lda #0
	sta seqEnable,X
	sta noteStepsLeft,X
	sta seqIndex,X
	lda #<testSequence
	sta seqBlockPtr,X
	lda #>testSequence
	sta seqBlockPtr+1,X
	lda #1
	sta seqEnable,X
	rts
.endproc


.proc _stopSound
	ldx #chSize*3
loop:
	lda #0
	sta seqEnable,X
	sta chSusTime,X
	sta chAtkTime,X
	cpx #0
	beq return
	txa
	sec
	sbc #chSize
	tax
	jmp loop
return:
	rts
.endproc

.proc _initSound
	lda #0
	sta AUDCTL
	lda #3
	sta SKCTL
	rts 
.endproc
