; sound.s

; Exports
.export _noteOn		; void __fastcall__ noteOn(UInt8 note);
.export _noteOff	; void __fastcall__ noteOff(void);
.export _startSequence ; void __fastcall__ startSequence(UInt8 song);
.export _stopSound	; void __fastcall__ stopSound(void);

.export _soundVBI	; called from immediateUserVBI
.export _initSound 	; called from initVBI


; Note and Octave constants
	NoteC  = 1
	NoteDb = 2
	NoteD  = 3
	NoteEb = 4
	NoteE  = 5
	NoteF  = 6
	NoteGb = 7
	NoteG  = 8
	NoteAb = 9
	NoteA  = 10
	NoteBb = 11
	NoteB  = 12
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


; =================================================================
; ==== Song: Once Upon A Time ====


testBlockT1:
	.byte NoteC+Oct5,  8, 2, 2 	; note, duration, volume, envelope
	.byte NoteC+Oct6,  8, 2, 2
	.byte NoteG+Oct5, 16, 2, 2

	.byte NoteF+Oct5,  8, 2, 2
	.byte NoteC+Oct6,  8, 2, 2
	.byte NoteC+Oct5, 16, 2, 2

	.byte NoteC+Oct5,  8, 2, 2
	.byte NoteF+Oct5,  8, 2, 2
	.byte NoteC+Oct6,  8, 2, 2
	.byte NoteD+Oct6,  8, 2, 2

	.byte NoteC+Oct6,  8, 2, 2
	.byte NoteG+Oct5,  8, 2, 2
	.byte NoteF+Oct5, 16, 2, 2

	.byte 0						; terminator

testBlockB1:
	.byte NoteF+Oct3, 16, 2, 1 	; note, duration, volume, envelope
	.byte NoteA+Oct3, 16, 2, 1
	
	.byte NoteAb+Oct3, 16, 2, 1
	.byte NoteG+Oct3, 16, 2, 1

	.byte 0						; terminator

testBlockT2:
	.byte NoteBb+Oct4, 1, 3, 11	; note, duration, volume, envelope
	.byte NoteB+Oct4,  1, 4, 11 
	.byte NoteC+Oct5,  8, 4, 1 
	.byte NoteC+Oct6,  4, 4, 1 
	.byte NoteA+Oct5,  4, 4, 1 
	.byte NoteG+Oct5,  4, 4, 1 
	.byte NoteF+Oct5,  4, 4, 1 
	.byte NoteE+Oct5,  4, 4, 1 
	.byte NoteF+Oct5,  4, 4, 1 

	.byte NoteG+Oct5,  8, 4, 1 
	.byte NoteE+Oct6,  6, 4, 1 
	.byte NoteEb+Oct6, 1, 3, 11 
	.byte NoteD+Oct6,  1, 4, 11 
	.byte NoteC+Oct6, 16, 4, 1 

	.byte NoteD+Oct5, 8, 4, 1 
	.byte NoteC+Oct6, 4, 4, 1 
	.byte NoteA+Oct5, 4, 4, 1 
	.byte NoteG+Oct5, 4, 4, 1 
	.byte NoteF+Oct5, 4, 4, 1 
	.byte NoteG+Oct5, 4, 4, 1 
	.byte NoteA+Oct5, 4, 4, 1 

	.byte NoteE+Oct5, 8, 4, 1 
	.byte NoteG+Oct5, 8, 4, 1 
	.byte NoteG+Oct5, 4, 4, 1 
	.byte NoteF+Oct5, 4, 4, 1 
	.byte NoteE+Oct5, 4, 4, 1 
	.byte NoteF+Oct5, 2, 4, 1 

	.byte 0						; terminator

testBlockB2:
	.byte $FF, 2, 0, 0		 	; rest
	.byte NoteG+Oct3, 4, 5, 1  	; note, duration, volume, envelope
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteG+Oct4, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteBb+Oct3,4, 5, 1 	;
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteG+Oct4, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1

	.byte NoteA+Oct3, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteG+Oct4, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteAb+Oct3,4, 5, 1 	;
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteG+Oct4, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1

	.byte NoteG+Oct3, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteG+Oct4, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteC+Oct4, 4, 5, 1	;
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteG+Oct4, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1

	.byte NoteA+Oct3, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteG+Oct4, 4, 5, 1
	.byte NoteF+Oct4, 4, 5, 1
	.byte NoteD+Oct4, 4, 5, 1	;
	.byte NoteD+Oct4, 4, 5, 1
	.byte NoteC+Oct4, 4, 5, 1
	.byte NoteC+Oct4, 2, 5, 1

	.byte 0						; terminator

testBlockEigthFiller:
	.byte $FF, 2, 0, 0
	.byte 0						; terminator

testBlockT3:
	.byte NoteC+Oct6, 4, 2, 2 	; note, duration, volume, envelope
	.byte NoteF+Oct5, 4, 2, 2 
	.byte NoteG+Oct5, 4, 2, 2 
	.byte NoteF+Oct5, 4, 2, 2 

	.byte NoteB+Oct4, 4, 2, 2 
	.byte NoteF+Oct5, 4, 2, 2 
	.byte NoteG+Oct5, 4, 2, 2 
	.byte NoteF+Oct5, 4, 2, 2 

	.byte NoteBb+Oct4, 4, 2, 2 
	.byte NoteF+Oct5, 4, 2, 2 
	.byte NoteG+Oct5, 4, 2, 2 
	.byte NoteBb+Oct5, 4, 2, 2 

	.byte NoteA+Oct5, 4, 2, 2 
	.byte NoteG+Oct5, 4, 2, 2 
	.byte NoteF+Oct5, 4, 2, 2 
	.byte NoteC+Oct5, 4, 2, 2 

	.byte 0						; terminator

testBlockB3:
	.byte NoteF+Oct4, 16, 3, 1 	; note, duration, volume, envelope
	.byte NoteG+Oct4, 16, 3, 1

	.byte NoteBb+Oct3,16, 3, 1
	.byte NoteF+Oct4, 16, 3, 1

	.byte 0						; terminator

testBlockRest:
	.byte $FF, 32, 0, 0
	.byte 0						; terminator

testBlockListT:
	.word testBlockT1
	.word testBlockT1

	.word testBlockT2
	.word testBlockT2
	.word testBlockEigthFiller

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
	.word testBlockEigthFiller

	.word testBlockB3
	.word testBlockB3

	.word testBlockRest
	.word testBlockRest
	.word testBlockRest
	.word testBlockRest

	.word testBlockRest
	.word 0


; =================================================================
; ==== Song: Percussion Test ====


testBlockP1:
	.byte NoteC+Oct3, 8, 6, 10 	; note, duration, volume, envelope
	.byte NoteC+Oct7, 8, 6, 10
	.byte NoteF+Oct4, 8, 6, 10
	.byte NoteC+Oct7, 8, 6, 10

	.byte 0						; terminator

testBlockListP:
	.word testBlockP1
	.word testBlockP1
	.word testBlockP1
	.word testBlockP1
	.word testBlockP1
	.word testBlockP1
	.word testBlockP1
	.word testBlockP1
	.word testBlockEigthFiller
	.word 0


; =================================================================
; ==== Song: Unnecessary Tension ====

song8_block1t:
	.byte NoteDb+Oct6, 2, 3, 10 	; note, duration, volume, envelope
	.byte NoteDb+Oct5, 2, 4, 10 
	.byte NoteDb+Oct5, 2, 4, 10 
	.byte NoteDb+Oct5, 2, 3, 10 
	.byte 0

song8_block2t:
	.byte NoteD+Oct6, 2, 3, 10
	.byte NoteD+Oct5, 2, 4, 10 
	.byte NoteD+Oct5, 2, 4, 10 
	.byte NoteD+Oct5, 2, 3, 10 
	.byte 0

song8_block1b:
	.byte NoteDb+Oct3, 5, 6, 1
	.byte NoteD+Oct3, 3, 7, 1 
	.byte $FF, 16, 0, 0 
	.byte NoteA+Oct3, 3, 8, 1 
	.byte $FF, 1, 0, 0 
	.byte NoteAb+Oct3, 3, 8, 1 
	.byte $FF, 1, 0, 0 

	.byte NoteDb+Oct3, 6, 7, 1
	.byte NoteD+Oct3, 2, 7, 1 
	.byte $FF, 5, 0, 0 
	.byte NoteGb+Oct3, 3, 7, 11 
	.byte NoteAb+Oct3, 16, 8, 1 
	.byte 0

song8_block2b:
	.byte NoteD+Oct3, 5, 6, 1
	.byte NoteEb+Oct3, 3, 7, 1 
	.byte $FF, 16, 0, 0 
	.byte NoteB+Oct3, 3, 8, 1 
	.byte $FF, 1, 0, 0 
	.byte NoteBb+Oct3, 3, 8, 1 
	.byte $FF, 1, 0, 0 

	.byte NoteD+Oct3, 6, 7, 1
	.byte NoteEb+Oct3, 2, 7, 1 
	.byte $FF, 5, 0, 0 
	.byte NoteBb+Oct3, 19, 8, 1 
	.byte 0

song8_blockListT:
	.word song8_block1t, song8_block1t, song8_block1t, song8_block1t
	.word song8_block1t, song8_block1t, song8_block1t, song8_block1t
	.word song8_block2t, song8_block2t, song8_block2t, song8_block2t
	.word song8_block2t, song8_block2t, song8_block2t, song8_block2t
	.word 0

song8_blockListB:
	.word song8_block1b
	.word song8_block2b
	.word 0

; Global variables

.segment "EXTZP": zeropage
ptrBlockList:
	.word 0
ptrBlock:
	.word 0
chStateOffset:
	.byte 0
tmpMultiply:
	.byte 0

.data
seqStepDur:			; controls tempo
	.byte 5 
seqTimer:
	.byte 0
vibratoTimer:
	.byte 0

; == Channel State ==
chNote: 			
	.byte 0
chCtrlMask: 			
	.byte 0
chCurLvl:			; current volume level
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
chSize = 16 		; bytes per channel state block
channelState1to3:
	.res chSize*3, $00


.code
	; POKEY 
	AUDF1 = $D200
	AUDC1 = $D201
	AUDF2 = $D202
	AUDC2 = $D203
	AUDF3 = $D204
	AUDC3 = $D205
	AUDF4 = $D206
	AUDC4 = $D207

.proc _soundVBI

sound:
	lda seqStepDur
	beq envelope_ch0 		; if seqStepDur == 0: sequencer is disabled

	ldx seqTimer				; 
	beq execute_seq_step 		; if seqTimer == 0: execute sequencer step
	dex							; else: seqTimer -= 1
	stx seqTimer
	jmp envelope_ch0

execute_seq_step:
	sta seqTimer 				; reset seqTimer = seqStepDur

step_ch0:
	ldx #0
	stx chStateOffset
	jsr _stepChannelSequencer

step_ch1:
	ldx #chSize*1
	stx chStateOffset
	jsr _stepChannelSequencer

step_ch2:
	ldx #chSize*2
	stx chStateOffset
	jsr _stepChannelSequencer

step_ch3:
	ldx #chSize*3
	stx chStateOffset
	jsr _stepChannelSequencer

envelope_ch0:
	ldx #0
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF1
	lda chCurLvl
	jsr _setWaveTableEnabled
	lda chCurLvl
	jsr _setWaveTableLevel
	;jsr _getPokeyAudCtrlValue ; channel 0 uses wavetable
	;sta AUDC1
	ldy #0
	jsr _setMissilePositionAtY	
	
envelope_ch1:
	ldx #chSize*1
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF2
	jsr _getPokeyAudCtrlValue
	sta AUDC2
	ldy #1
	jsr _setMissilePositionAtY	
	
envelope_ch2:
	ldx #chSize*2
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF3
	jsr _getPokeyAudCtrlValue
	sta AUDC3
	ldy #2
	jsr _setMissilePositionAtY	
	
envelope_ch3:
	ldx #chSize*3
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF4
	jsr _getPokeyAudCtrlValue
	sta AUDC4
	ldy #3
	jsr _setMissilePositionAtY	
	
vibrato_timer:
	lda vibratoTimer 		; vibratoTimer += 1
	clc
	adc #1
	and #$07 				; keep vibratoTimer in range from 0 to 7
	sta vibratoTimer

	rts
.endproc


.proc _stepChannelSequencer
	; on entry: chStateOffset are set
	ldx chStateOffset
	lda seqEnable,X			; if seqEnable == 0: return
	beq return

	lda noteStepsLeft,X		; if noteStepsLeft == 0: execute next step in sequence
	beq execute_next_step

	sec  					; noteStepsLeft -= 1
	sbc #1
	sta noteStepsLeft,X	
return:
	rts			

execute_next_step:
	lda blockListPtr,X		; ptrBlockList = chState->blockListPtr
	sta ptrBlockList
	lda blockListPtr+1,X
	sta ptrBlockList+1

get_block_ptr:
	lda blockIndex,X		; get blockIndex
	asl a
	tay 					; Y = blockIndex
	lda (ptrBlockList),Y
	sta ptrBlock
	iny
	lda (ptrBlockList),Y
	sta ptrBlock+1

	bne get_note 			; if ptrBlock == NULL: reset blockIndex
	cpy #1
	beq return				; if pointer at first index was NULL: return
	lda #0
	sta blockIndex,X		; else: blockIndex = 0; noteIndex = 0
	sta noteIndex,X
	jmp get_block_ptr
get_note:
	lda noteIndex,X	 		; offset = noteIndex * 4
	asl a					; sequencer note unit is 4 bytes
	asl a					; 
	tay						; use Y for offset from block start
	lda (ptrBlock),Y		; get noteNumber
	bne save_note_number	; if noteNumber == 0: jump to next block
jump_to_next_block:
	inc blockIndex,X		; blockIndex += 1
	lda #0
	sta noteIndex,X			; noteIndex = 0
	jmp get_block_ptr
save_note_number:
	sta chNote,X 			; save note number
get_duration:	
	iny				
	lda (ptrBlock),Y 		
	sec
	sbc #1					; duration -= 1
	sta noteStepsLeft,X
get_volume:
	iny
	lda (ptrBlock),Y 		
	sta chSusLvl,X
get_envelope:
	iny
	lda (ptrBlock),Y 		
	jsr _setEnvelope
inc_note_index:
	inc noteIndex,X
	rts
.endproc

.proc _setEnvelope
	pha
	ldx chStateOffset
	lda #0
	sta chCurLvl,X 			; all envelopes start at currentLevel = 0

	pla
	cmp #1
	beq envelope_1
	cmp #2
	beq envelope_2
	cmp #10
	beq envelope_10
	cmp #11
	beq envelope_11
	jmp default_envelope

envelope_1: 				; == Soft pad envelope ==
	lda #2
	sta chAtkTime,X			; fast attack to sustain level

	lda chSusLvl, X
	lsr a
	sta chAtkRate,X

	lda #1
	sta chDecRate,X 		; slow decay and release
	sta chRelRate,X

	lda chSusLvl, X
	jsr _calculateSustainTimeMinusA
	rts

envelope_2: 				; == Typical melody envelope ==
	lda #2		 			; fast attack
	sta chAtkTime,X
	lda #4
	sta chAtkRate,X

	lda #2
	sta chDecRate,X 		; fast decay and release
	sta chRelRate,X

	lda #5	
	jsr _calculateSustainTimeMinusA
	rts

envelope_10: 				; == Percussion envelope, no sustain ==
	lda #0		 			; no attack
	sta chAtkTime,X

	lda #1
	sta chAtkRate,X
	sta chDecRate,X 		; slow decay and release
	sta chRelRate,X

	lda #0	
	jsr _calculateSustainTimeMinusA
	rts

envelope_11: 				; == Soft pad envelope with sustain ==
	lda #2
	sta chAtkTime,X			; fast attack to sustain level

	lda chSusLvl, X
	lsr a
	sta chAtkRate,X
	sta chDecRate,X 		; slow decay and release
	sta chRelRate,X

	lda #0
	jsr _calculateSustainTimeMinusA
	rts

default_envelope:
	lda #0
	sta chAtkTime,X 		; no attack/decay
	sta chAtkRate,X
	sta chDecRate,X

	lda #2
	sta chRelRate,X 		; fast release

	lda #0	
	jsr _calculateSustainTimeMinusA
	rts
.endproc


.proc _calculateSustainTimeMinusA
	; returns noteSteps * seqStepDur - A
	pha 
	ldx chStateOffset
	lda noteStepsLeft,X 	; multiply note steps * step duration	
	ldx seqStepDur
	jsr _multiplyAX

	tsx 
	sec 
	sbc $0101,X
	bcs set_sustain_time	; if sus_time < 0: sus_time = 0
	lda #0
set_sustain_time:
	inx  					; pop stack
	txs 

	ldx chStateOffset
	sta chSusTime,X

	rts
.endproc

.proc _multiplyAX
	; returns A * X in A
	sta tmpMultiply 	; save original value in tmpMultiply
	lda #0
	jmp while
loop:
	tay
	txa 
	lsr a 
	tax 
	tya
	bcc continue
	clc
	adc tmpMultiply
continue:
	asl tmpMultiply
while:
	cpx #0
	bne loop
	rts
.endproc

.proc _stepChannelEnvelope ; 
	ldx chStateOffset

attack:
	lda chAtkTime,X 	; if atk_time != 0: apply attack
	beq decay

	sec
	sbc #1
	sta chAtkTime,X		; atk_time -= 1

	clc
	lda chCurLvl,X
	adc chAtkRate,X		; cur_lvl += atk_rate
	sta chCurLvl,X
	rts 				; return

decay:
	lda chCurLvl,X
	cmp chSusLvl,X		; if cur_lvl > sus_lvl: apply decay
	beq sustain
	bcc sustain

	sec
	sbc chDecRate,X 	; cur_lvl -= dec_rate
	sta chCurLvl,X
	rts 				; return

sustain:
	lda chSusTime,X		; if sus_time != 0: wait for sustain
	beq release	

	sec
	sbc #1				; sus_time -= 1
	sta chSusTime,X
	lda chSusLvl,X 		; cur_lvl = sus_lvl
	sta chCurLvl,X
	rts					; return

release:
	lda chCurLvl,X
	cmp #0 						; if cur_lvl != 0: apply release
	beq return
	sec
	sbc chRelRate,X
	bcs set_release_level
	lda #0
set_release_level:
	sta chCurLvl,X
return:
	rts
.endproc


.proc _getPokeyAudFreqValue
	; uses chStateOffset & vibratoTimer to return AUDF# value
	ldx chStateOffset
	lda chNote,X 			; X = note
	asl a
	tax
	lda noteTable-2,X		; freq = noteTable[note].audf
	tay 
	lda vibratoTimer 		; if vibratoTimer < vibr: increment freq
	cmp noteTable-1,X 		; vibr = noteTable[note].vibr
	bcs return
	iny
return:
	tya 
	rts
.endproc

.proc _getPokeyAudCtrlValue
	ldx chStateOffset
	lda chCurLvl,X
	cmp #$10
	bcc below_limit
	lda #$0F
below_limit:
	and #$0F
	ora chCtrlMask,X
	rts
.endproc

.data
waveLength:
	.byte 4
waveIndex:
	.byte 0
waveTable:
	.byte 0, 0, 0, 0

.code
.proc _waveTableTimerInterrupt
	txa
	pha

	ldx waveIndex
	lda waveTable,X
	ora #$10
	sta AUDC1

	inx 
	cpx waveLength
	bne store_index
	ldx #0
store_index:
	stx waveIndex

return:
	pla
	tax
	pla
	rti
.endproc

.proc _setWaveTableLevel
	cmp #$0F
	bcc skip_limiter
	lda #$0F
skip_limiter:
	sta waveTable+2
	lsr a
	sta waveTable+1
	sta waveTable+3
	rts
.endproc


.proc _setWaveTableEnabled
	POKMSK = $10
	STIMER = $D209
	IRQEN  = $D20E

	cmp #0
	bne set_enabled

set_disabled:
	lda #$C0 
	sta POKMSK
	sta IRQEN
	rts

set_enabled:
	lda POKMSK
	cmp #$C1
	beq return
	lda #$C1 
	sta POKMSK
	sta IRQEN
	sta STIMER
return:
	rts
.endproc


.proc _setMissilePositionAtY
	; reads channel state and updates missile horizontal position

	; GTIA
	HPOSM0 = $D004		; Missile 0 horizontal position
	HPOSM1 = $D005		; Missile 1
	HPOSM2 = $D006		; Missile 2
	HPOSM3 = $D007		; Missile 3

	ldx chStateOffset
	lda chCurLvl,X
	beq set_position
	lda chNote,X		; get note
	asl a				; else: note = note * 2 + 46
	clc
	adc #46 			; since notes start from 1, while 48 is the normal left edge
set_position:
	sta HPOSM0,Y
return:
	rts
.endproc


.proc _noteOn		 			
	; on entry: A=note number
	ldx #chSize*3
	stx chStateOffset

	sta chNote,X

	lda #$E0
	sta chCtrlMask,X

	lda #8
	sta noteStepsLeft,X

	lda #7
	sta chSusLvl,X

	lda #11
	jsr _setEnvelope

	rts
.endproc


.proc _noteOff
	ldx #chSize*3
	lda #0
	sta chSusTime,X
	sta chAtkTime,X
	jsr _setWaveTableLevel

	lda #0
	jsr _setWaveTableEnabled

	rts
.endproc


.proc _startSequence
	tay 					; save song parameter in Y

	ldx #chSize*4 			; reset all channels
loop_init:
	txa 					; X -= chSize
	sec
	sbc #chSize
	tax

	lda #0
	sta seqEnable,X 	 	; disable sequencer		
	sta noteStepsLeft,X
	sta noteIndex,X
	sta blockIndex,X

	lda #$E0
	sta chCtrlMask,X 		; tone

	cpx #0
	bne loop_init

switch:
	cpy #1
	beq play_song_1
	jmp default

play_song_1:
	lda #3
	sta seqStepDur 				; fast tempo

	lda #<song8_blockListB
	sta blockListPtr+chSize*0
	lda #>song8_blockListB
	sta blockListPtr+chSize*0+1

	lda #<song8_blockListT
	sta  blockListPtr+chSize*1
	lda #>song8_blockListT
	sta  blockListPtr+chSize*1+1

	lda #1 						; enable channels that are used
	sta seqEnable+chSize*0
	sta seqEnable+chSize*1
	rts

default:
	lda #5
	sta seqStepDur 				; medium tempo

	lda #<testBlockListT
	sta blockListPtr+chSize*1
	lda #>testBlockListT
	sta blockListPtr+chSize*1+1

	lda #<testBlockListB
	sta  blockListPtr+chSize*2
	lda #>testBlockListB
	sta  blockListPtr+chSize*2+1

	lda #1 						; enable channels that are used
	sta seqEnable+chSize*1
	sta seqEnable+chSize*2
	rts
.endproc


.proc _stopSound
	lda #0
	jsr _setWaveTableEnabled
	lda #0
	jsr _setWaveTableLevel

	ldx #chSize*3
loop:
	lda #0
	sta seqEnable,X
	sta chSusTime,X
	sta chAtkTime,X
	sta noteStepsLeft,X
	sta noteIndex,X
	sta blockIndex,X
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
	VTIMR1 = $0210
	AUDCTL = $D208
	SKCTL  = $D20F

	lda #<_waveTableTimerInterrupt
	sta VTIMR1
	lda #>_waveTableTimerInterrupt
	sta VTIMR1+1

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
