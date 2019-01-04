; sound.s

; Exports
.export _noteOn		; void __fastcall__ noteOn(UInt8 note);
.export _noteOff	; void __fastcall__ noteOff(void);
.export _startSequence ; void __fastcall__ startSequence(void);
.export _stopSound	; void __fastcall__ stopSound(void);

.export _soundVBI	; called from immediateUserVBI
.export _initSound 	; called from initVBI

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
vibratoTimer:
	.byte 0
seqTimer:
	.byte 0
seqStepDur:			; controls tempo
	.byte 5 

chNote:
	.byte 0
chFreq: 		
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

chSize = 16 		; bytes per channel state block

channelState1to3:
	.res chSize*3, $00


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

	.byte 0						; terminator

testBlockB1:
	.byte NoteF+Oct3, 16, 2, 0 	; note, duration, volume, envelope
	.byte NoteA+Oct3, 16, 2, 0
	
	.byte NoteAb+Oct3, 16, 2, 0
	.byte NoteG+Oct3, 16, 2, 0

	.byte 0						; terminator

testBlockT2:
	.byte NoteBb+Oct4, 1, 8, 0	; note, duration, volume, envelope
	.byte NoteB+Oct4, 1, 10, 0 
	.byte NoteC+Oct5, 8, 4, 1 
	.byte NoteC+Oct6, 4, 2, 1 
	.byte NoteA+Oct5, 4, 2, 1 
	.byte NoteG+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 
	.byte NoteE+Oct5, 4, 2, 1 
	.byte NoteF+Oct5, 4, 2, 1 

	.byte NoteG+Oct5, 8, 2, 1 
	.byte NoteE+Oct6, 6, 2, 1 
	.byte NoteEb+Oct6, 1, 8, 0 
	.byte NoteD+Oct6, 1, 10, 0 
	.byte NoteC+Oct6, 16, 4, 1 

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
	.byte NoteF+Oct5, 2, 2, 1 

	.byte 0						; terminator

testBlockB2:
	.byte $FF, 2, 0, 0		 	; rest
	.byte NoteG+Oct3, 4, 2, 1  	; note, duration, volume, envelope
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteBb+Oct3, 4, 2, 1 	;
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1

	.byte NoteA+Oct3, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteAb+Oct3, 4, 2, 1 	;
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1

	.byte NoteG+Oct3, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteC+Oct4, 4, 2, 1	;
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1

	.byte NoteA+Oct3, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteG+Oct4, 4, 2, 1
	.byte NoteF+Oct4, 4, 2, 1
	.byte NoteD+Oct4, 4, 2, 1	;
	.byte NoteD+Oct4, 4, 2, 1
	.byte NoteC+Oct4, 4, 2, 1
	.byte NoteC+Oct4, 2, 2, 1

	.byte 0						; terminator

testBlockEigthFiller:
	.byte $FF, 2, 0, 0
	.byte 0						; terminator

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

	.byte 0						; terminator

testBlockB3:
	.byte NoteF+Oct4, 16, 2, 0 	; note, duration, volume, envelope
	.byte NoteG+Oct4, 16, 2, 1

	.byte NoteBb+Oct3, 16, 2, 0
	.byte NoteF+Oct4, 16, 2, 0

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

.code

.proc _soundVBI

; ===== Sound Section =====

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

envelope_ch0:
	ldx #0
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyValues
	sta AUDC1
	stx AUDF1
	ldy #0
	jsr _setMissilePositionAtY	
	
envelope_ch1:
	ldx #chSize*1
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyValues
	sta AUDC2
	stx AUDF2
	ldy #1
	jsr _setMissilePositionAtY	
	
envelope_ch2:
	ldx #chSize*2
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyValues
	sta AUDC3
	stx AUDF3
	ldy #2
	jsr _setMissilePositionAtY	
	
envelope_ch3:
	ldx #chSize*3
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyValues
	sta AUDC4
	stx AUDF4
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
	bne save_note_number 	; if noteNumber == 0: jump to next block
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
set_freq_vibr:
	lda chNote,X 			; get note number
	asl a					; noteTable has 2 bytes per item
	tay
	lda noteTable-2,Y		; freq = noteTable[Y].audf
	sta chFreq,X
	lda noteTable-1,Y		; vibr = noteTable[Y].vibrato
	sta chVibr,X
inc_note_index:
	inc noteIndex,X
	rts
.endproc

.proc _setEnvelope
	ldx chStateOffset
	cmp #1
	beq envelope_1
	cmp #2
	beq envelope_2
	jmp default_envelope

envelope_1:
	lda #7
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
	sta chAtkTime,X 		; no attack/decay
	sta chAtkRate,X
	sta chDecRate,X

	lda #2
	sta chRelRate,X 		; fast release

	lda chSusLvl,X			; start at currentLevel = sustainLevel
	sta chCurLvl,X

	lda noteStepsLeft,X 	; multiply sustain time by step duration	
	jsr _multiplyByStepDuration
	;jmp set_sustain_time

set_sustain_time:
	ldx chStateOffset
	sta chSusTime,X
return:
	rts
.endproc

.export _multiplyByStepDuration
.proc _multiplyByStepDuration
	; A = A * step duration
	sta tmpMultiply 	; save original value in tmpMultiply
	ldy seqStepDur
	lda #0
	jmp while
loop:
	tax
	tya 
	lsr a 
	tay 
	txa
	bcc continue
	clc
	adc tmpMultiply
continue:
	asl tmpMultiply
while:
	cpy #0
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


.proc _getPokeyValues
	; on entry: chStateOffset, vibratoTimer, and channel state are set
	ldy chStateOffset
	lda vibratoTimer
	cmp chVibr,Y 		; if vibratoTimer < chVibr: increment audf value
	lda chFreq,Y
	bcs audf_to_x
	adc #1
audf_to_x:
	tax

audctrl:
	ldy chStateOffset
	lda chCurLvl,Y
	cmp #$10
	bcc below_limit
	lda #$0F
below_limit:
	and #$0F
	ora #$E0
	rts
.endproc


ldy #0
.proc _setMissilePositionAtY
	; reads channel state and updates missile horizontal position
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
	ldx #chSize*3 		; use channel 3		

	sta chNote,X

	asl a					; noteTable has 2 bytes per item
	tay
	lda noteTable-2,Y		; freq = noteTable[Y].audf
	sta chFreq,X
	lda noteTable-1,Y		; vibr = noteTable[Y].vibrato
	sta chVibr,X

	lda #16
	sta noteStepsLeft,X

	lda #8
	sta chSusLvl,X

	lda #1
	jsr _setEnvelope
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