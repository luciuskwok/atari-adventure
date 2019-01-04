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

	; GTIA
	HPOSM0 = $D004		; Missile 0 horizontal position
	HPOSM1 = $D005		; Missile 1
	HPOSM2 = $D006		; Missile 2
	HPOSM3 = $D007		; Missile 3

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

testBlockT1:
	.byte NoteC+Oct5, 8, 2, 1 	; note, duration, volume, envelope
	.byte NoteC+Oct6, 8, 2, 1
	.byte NoteG+Oct5, 16, 2, 1

	.byte NoteF+Oct5, 8, 2, 1
	.byte NoteC+Oct6, 8, 2, 1
	.byte NoteC+Oct5, 16, 2, 1

	.byte NoteC+Oct5, 8, 2, 1
	.byte NoteF+Oct5, 8, 2, 1
	.byte NoteC+Oct6, 8, 2, 1
	.byte NoteD+Oct6, 8, 2, 1

	.byte NoteC+Oct6, 8, 2, 1
	.byte NoteG+Oct5, 8, 2, 1
	.byte NoteF+Oct5, 16, 2, 1

	.byte 0, 0, 0, 0 			; terminator

testBlockB1:
	.byte NoteF+Oct3, 16, 2, 1 	; note, duration, volume, envelope
	.byte NoteA+Oct3, 16, 2, 1
	
	.byte NoteAb+Oct3, 16, 2, 1
	.byte NoteG+Oct3, 16, 2, 1

	.byte 0, 0, 0, 0 			; terminator

testBlockT2:
	.byte NoteBb+Oct4, 1, 2, 1 	; note, duration, volume, envelope
	.byte NoteB+Oct4, 1, 2, 1 
	.byte NoteC+Oct5, 8, 4, 1 
	.byte NoteC+Oct6, 4, 2, 1 
	.byte NoteA+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteE+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 

	.byte NoteG+Oct5, 8, 2, 1 
	.byte NoteE+Oct6, 8, 2, 1 
	.byte NoteEb+Oct6, 1, 2, 1 
	.byte NoteD+Oct6, 1, 2, 1 
	.byte NoteC+Oct6, 14, 4, 1 

	.byte NoteD+Oct5, 8, 2, 1 
	.byte NoteC+Oct6, 4, 2, 1 
	.byte NoteA+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteA+Oct5, 4, 2, 1 

	.byte NoteE+Oct5, 8, 2, 1 
	.byte NoteG+Oct5, 8, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteE+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 

	.byte 0, 0, 0, 0 			; terminator

testBlockB2:
	.byte 0, 2, 0, 0		 	; rest
	.byte NoteG+Oct3, 8, 2, 1  	; note, duration, volume, envelope
	.byte NoteF+Oct4, 8, 2, 1
	.byte NoteG+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1

	.byte NoteBb+Oct3, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1
	.byte NoteG+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1

	.byte NoteA+Oct3, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1
	.byte NoteG+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1

	.byte NoteAb+Oct3, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1
	.byte NoteG+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1

	.byte NoteG+Oct3, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1
	.byte NoteG+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1

	.byte NoteC+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1
	.byte NoteG+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1

	.byte NoteA+Oct3, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1
	.byte NoteG+Oct4, 8, 2, 1
	.byte NoteF+Oct4, 8, 2, 1

	.byte NoteD+Oct4, 8, 2, 1
	.byte NoteD+Oct4, 8, 2, 1
	.byte NoteC+Oct4, 8, 2, 1
	.byte NoteC+Oct4, 8, 2, 1

	.byte 0, 0, 0, 0 			; terminator

testBlockT3:
	.byte NoteC+Oct6, 4, 2, 1 	; note, duration, volume, envelope
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 

	.byte NoteB+Oct4, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 

	.byte NoteBb+Oct4, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteBb+Oct5, 4, 2, 1 

	.byte NoteA+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteC+Oct5, 4, 2, 1 

	.byte 0, 0, 0, 0 			; terminator

testBlockB3:
	.byte NoteA+Oct3, 16, 2, 1 	; note, duration, volume, envelope
	.byte NoteBb+Oct3, 16, 2, 1

	.byte NoteD+Oct3, 16, 2, 1
	.byte NoteA+Oct3, 16, 2, 1

	.byte 0, 0, 0, 0 			; terminator

testBlockRest:
	.byte 0, 32, 0, 0
	.byte 0, 0, 0, 0 			; terminator

testBlockListT:
	.word testBlockT1
	.word testBlockT1

	.word testBlockT2
	.word testBlockT2

	.word testBlockT3
	.word testBlockT3

	.word testBlockT3
	.word testBlockT3

	.word testBlockRest
	.word 0

testBlockListB:
	.word testBlockB1
	.word testBlockB1
	.word testBlockB1
	.word testBlockB1

	.word testBlockB2
	.word testBlockB2

	.word testBlockB3
	.word testBlockB3

	.word testBlockRest
	.word testBlockRest

	.word testBlockRest
	.word 0

.segment "EXTZP": zeropage
ptrBlockList:
	.word 0
ptrBlock:
	.word 0
tmpNoteNumber:
	.byte 0

.data
vibratoTimer:
	.byte 0
seqTimer:
	.byte 0
seqStepDur:		; controls tempo
	.byte 4 

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
noteStepsLeft:
	.byte 0
noteIndex:
	.byte 0
blockIndex:
	.byte 0
blockListPtr:
	.word 0

chSize = 15 ; bytes per channel state block

channelState1to3:
	.res chSize*3, $00


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

	lda #$FF
	sta tmpNoteNumber			; $FF = unchanged for debugging

	ldx #0 						; channel 0 sequencer
	jsr _stepSequenceAtX

	lda tmpNoteNumber
	cmp #$FF
	beq step_ch1
	asl a						; set missile 0 position for debugging
	clc
	adc #48
	sta HPOSM0

step_ch1:
	lda #$FF
	sta tmpNoteNumber			; $FF = unchanged for debugging

	ldx #chSize*1 				; channel 1 sequencer
	jsr _stepSequenceAtX

	lda tmpNoteNumber
	cmp #$FF
	beq apply_envelopes
	asl a						; set missile 1 position for debugging
	clc
	adc #48
	sta HPOSM1

apply_envelopes:
	ldy #0						; channel 0 envelope
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF1
	sta AUDC1

	and #$0F					; if volume of ch.3 == 0:
	bne apply_envelope_ch1		; hide missile 0 for debugging
	lda #0
	;sta HPOSM0					

apply_envelope_ch1:
	ldy #chSize*1				; channel 1 envelope
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF2
	sta AUDC2

	and #$0F					; if volume of ch.3 == 0:
	bne apply_envelope_ch2		; hide missile 1 for debugging
	lda #0
	;sta HPOSM1					

apply_envelope_ch2:
	ldy #chSize*2				; channel 2 envelope
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF3
	sta AUDC3

apply_envelope_ch3:
	ldy #chSize*3				; channel 3 envelope
	jsr _applyEnvelopeAtY
	jsr _getAudFreqCtrlAtY
	stx AUDF4
	sta AUDC4

	and #$0F					; if volume of ch.3 == 0:
	bne inc_vibrato_timer		; hide missile 3 for debugging
	lda #0
	sta HPOSM3					

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
	lda blockListPtr,X		; ptrSeqBlock = chState[X].blockListPtr
	sta ptrBlockList
	lda blockListPtr+1,X
	sta ptrBlockList+1

get_block_ptr:
	lda blockIndex,X
	asl a
	tay
	lda (ptrBlockList),Y
	sta ptrBlock
	iny
	lda (ptrBlockList),Y
	sta ptrBlock+1

	bne get_note 			; if ptrBlock == NULL: reset blockIndex
	cpy #0
	beq return				; if blockIndex == 0: return
	lda #0
	sta blockIndex,X		; else: blockIndex = 0; noteIndex = 0
	sta noteIndex,X
	jmp get_block_ptr

get_note:
	lda noteIndex,X 		; offset = noteIndex * 4
	asl a					; count is 1 byte
	asl a					; sequencer note unit is 4 bytes

	tay						; use Y for offset from block start
	lda (ptrBlock),Y		; get note number
	sta tmpNoteNumber		; save note number

	iny						; get duration
	lda (ptrBlock),Y 
	bne set_duration		; if duration == 0: jump to next block
	inc blockIndex,X
	lda #0
	sta noteIndex,X	
	jmp get_block_ptr

set_duration:
	sta noteStepsLeft,X

	iny
	lda (ptrBlock),Y 		; get volume
	sta chSusLvl,X

	iny
	lda (ptrBlock),Y 		; get envelope
	jsr _setEnvelopeAtX

	lda tmpNoteNumber		; get note number again
	jsr _setNoteAtX 		; uses Y, so offset needed to be stored

	inc noteIndex,X
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
	sta chAtkRate,X 		; fast attack

	lda #2
	sta chAtkTime,X

	lda #1
	sta chDecRate,X 		; slow decay and release
	sta chRelRate,X

	lda #0
	sta chCurLvl,X 			; start at currentLevel = 0

	lda noteStepsLeft,X 	; multiply step count by step duration	
	jsr _multiplyByStepDuration
	sec
	sbc #$11				; and subtract time for atk, dec,& rel
	bcs set_sustain_time 	; if sus_time < 0: sus_time = 0
	lda #0
	jmp set_sustain_time

envelope_2:
	lda #15
	sta chAtkTime,X			; slow attack

	lda #1
	sta chAtkRate,X
	sta chDecRate,X 		; slow decay and release
	sta chRelRate,X

	lda #0
	sta chCurLvl,X 			; start at currentLevel = 0

	lda noteStepsLeft,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	sec
	sbc #$1E				; and subtract time for atk, dec,& rel
	bcs set_sustain_time 	; if sus_time < 0: sus_time = 0
	lda #0
	jmp set_sustain_time

default_envelope:
	lda #0
	sta chAtkTime,X 		; no attack
	sta chAtkRate,X

	lda #15
	sta chDecRate,X 		; instant decay & release
	sta chRelRate,X

	lda chSusLvl,X			; start at currentLevel = sustainLevel
	sta chCurLvl,X

	lda noteStepsLeft,X 	; multiply sustain time by step duration	
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

	pha 				; store note number for later

	asl a 				; set missile 3 for debugging
	clc
	adc #48
	sta HPOSM3

	pla
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
	sta HPOSM3
	rts
.endproc


.proc _startSequence

	ldx #0		 				; channel 0
	lda #0
	sta seqEnable,X 			
	sta noteStepsLeft,X
	sta noteIndex,X
	sta blockIndex,X
	lda #<testBlockListT
	sta blockListPtr,X
	lda #>testBlockListT
	sta blockListPtr+1,X

	ldx #chSize*1		 		; channel 1
	lda #0
	sta seqEnable,X 			
	sta noteStepsLeft,X
	sta noteIndex,X
	sta blockIndex,X
	lda #<testBlockListB
	sta blockListPtr,X
	lda #>testBlockListB
	sta blockListPtr+1,X

	lda #1 						; enable sequencer
	ldx #0		 				; channel 0
	sta seqEnable,X
	ldx #chSize		 			; channel 1
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
	sta seqEnable+chSize*0
	sta seqEnable+chSize*1
	sta seqEnable+chSize*2
	sta seqEnable+chSize*3
	lda #3
	sta SKCTL
	rts 
.endproc
