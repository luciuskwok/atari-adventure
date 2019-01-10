; sound.asm


.export _soundVBI	; called from immediateUserVBI
.export _updateSoundSprites	; called from deferredUserVBI
.export _initSound 	; called from initVBI
.importzp sp

; Note and Octave constants
	Rest   = $FF
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
noteTableLength = 49

; =================================================================
; ==== Song 0: Once Upon A Time ====


song0_block1t:
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

song0_block1b:
	.byte NoteF+Oct3, 16, 2, 1 	; note, duration, volume, envelope
	.byte NoteA+Oct3, 16, 2, 1
	
	.byte NoteAb+Oct3, 16, 2, 1
	.byte NoteG+Oct3, 16, 2, 1

	.byte 0						; terminator

song0_block2t:
	.byte NoteBb+Oct4, 1, 3, 3	; note, duration, volume, envelope
	.byte NoteB+Oct4,  1, 4, 3 
	.byte NoteC+Oct5,  8, 4, 1 
	.byte NoteC+Oct6,  4, 4, 1 
	.byte NoteA+Oct5,  4, 4, 1 
	.byte NoteG+Oct5,  4, 4, 1 
	.byte NoteF+Oct5,  4, 4, 1 
	.byte NoteE+Oct5,  4, 4, 1 
	.byte NoteF+Oct5,  4, 4, 1 

	.byte NoteG+Oct5,  8, 4, 1 
	.byte NoteE+Oct6,  6, 4, 1 
	.byte NoteEb+Oct6, 1, 3, 3 
	.byte NoteD+Oct6,  1, 4, 3 
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

song0_block2b:
	.byte Rest, 2, 0, 0		 	; rest
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

song0_block3t:
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

song0_block3b:
	.byte NoteF+Oct4, 16, 3, 1 	; note, duration, volume, envelope
	.byte NoteG+Oct4, 16, 3, 1

	.byte NoteBb+Oct3,16, 3, 1
	.byte NoteF+Oct4, 16, 3, 1

	.byte 0						; terminator

song0_eighthRest:
	.byte Rest, 2, 0, 0
	.byte 0						; terminator

song0_wholeRest:
	.byte Rest, 32, 0, 0
	.byte 0						; terminator

song0_blockListT: 				; treble
	.word song0_block1t
	.word song0_block1t

	.word song0_block2t
	.word song0_block2t
	.word song0_eighthRest

	.word song0_block3t
	.word song0_block3t

	.word song0_block3t
	.word song0_block3t

	.word song0_wholeRest
	.word 0

song0_blockListB: 				; bass 
	.word song0_block1b
	.word song0_block1b
	.word song0_block1b
	.word song0_block1b

	.word song0_block2b
	.word song0_block2b
	.word song0_eighthRest

	.word song0_block3b
	.word song0_block3b

	.word song0_wholeRest
	.word song0_wholeRest
	.word song0_wholeRest
	.word song0_wholeRest

	.word song0_wholeRest
	.word 0

; =================================================================
; ==== Song 4: Fallen Down ====

song4_env = 4
song4_vol = 4

song4_block1t:
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteDb+Oct5, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteDb+Oct5, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteDb+Oct5, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block1u:
	.byte NoteD +Oct4, 2, song4_vol, song4_env
	.byte NoteGb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteA +Oct4, 2, song4_vol, song4_env
	.byte NoteGb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteA +Oct4, 2, song4_vol, song4_env
	.byte NoteGb+Oct4, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block3t:
	.byte NoteB +Oct4, 2, song4_vol, song4_env
	.byte NoteA +Oct4, 2, song4_vol-1, song4_env
	.byte NoteDb+Oct5, 3, song4_vol+1, song4_env
	.byte Rest, 1, 0, 0
	.byte NoteA +Oct4, 2, song4_vol, song4_env
	.byte NoteB +Oct4, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block3u:
	.byte NoteB +Oct3, 2, song4_vol, song4_env
	.byte NoteEb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct4, 2, song4_vol, song4_env
	.byte NoteEb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct4, 2, song4_vol, song4_env
	.byte NoteEb+Oct4, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block4t:
	.byte NoteE +Oct5, 2, song4_vol, song4_env
	.byte NoteEb+Oct5, 2, song4_vol-1, song4_env
	.byte NoteE +Oct5, 2, song4_vol, song4_env
	.byte NoteGb+Oct5, 2, song4_vol-1, song4_env
	.byte NoteEb+Oct5, 2, song4_vol, song4_env
	.byte NoteB +Oct4, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block5t:
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteB +Oct4, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteB +Oct4, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteB +Oct4, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block5u:
	.byte NoteG +Oct3, 2, song4_vol, song4_env
	.byte NoteB +Oct3, 2, song4_vol-1, song4_env
	.byte NoteD +Oct4, 2, song4_vol, song4_env
	.byte NoteB +Oct3, 2, song4_vol-1, song4_env
	.byte NoteD +Oct4, 2, song4_vol, song4_env
	.byte NoteB +Oct3, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block6t:
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteBb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteBb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteG +Oct5, 3, song4_vol+1, song4_env
	.byte Rest, 1, 0, 0
	.byte 0 	; note, duration, volume, envelope

song4_block6u:
	.byte NoteG +Oct3, 2, song4_vol, song4_env
	.byte NoteBb+Oct3, 2, song4_vol-1, song4_env
	.byte NoteD +Oct4, 2, song4_vol, song4_env
	.byte NoteBb+Oct3, 2, song4_vol-1, song4_env
	.byte NoteD +Oct4, 2, song4_vol, song4_env
	.byte NoteBb+Oct3, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block7t:
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteD +Oct5, 2, song4_vol-1, song4_env
	.byte NoteGb+Oct5, 2, song4_vol, song4_env
	.byte NoteD +Oct5, 2, song4_vol-1, song4_env
	.byte NoteE +Oct5, 2, song4_vol, song4_env
	.byte NoteGb+Oct5, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block7u:
	.byte NoteD +Oct4, 2, song4_vol, song4_env
	.byte NoteGb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteA +Oct4, 2, song4_vol, song4_env
	.byte NoteGb+Oct4, 2, song4_vol-1, song4_env
	.byte NoteA +Oct4, 2, song4_vol, song4_env
	.byte NoteGb+Oct4, 2, song4_vol-1, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_block8t:
	.byte NoteE +Oct5, 3, song4_vol+1, song4_env
	.byte Rest, 1, 0, 0
	.byte NoteD +Oct5, 3, song4_vol, song4_env
	.byte Rest, 1, 0, 0
	.byte NoteDb+Oct5, 3, song4_vol-1, song4_env
	.byte Rest, 1, 0, 0
	.byte 0 	; note, duration, volume, envelope

song4_block8u:
	.byte NoteDb+Oct4, 2, song4_vol+1, song4_env
	.byte NoteE +Oct4, 2, song4_vol, song4_env
	.byte NoteA +Oct4, 2, song4_vol, song4_env
	.byte NoteE +Oct4, 2, song4_vol-1, song4_env
	.byte NoteA +Oct4, 2, song4_vol-1, song4_env
	.byte NoteE +Oct4, 2, song4_vol-2, song4_env
	.byte 0 	; note, duration, volume, envelope

song4_measureRest:
	.byte Rest, 12, 0, 0
	.byte 0

song4_block8b1:
	.byte Rest, 8, 0, 0
	.byte NoteA +Oct4, 4, 8, 3
	.byte 0

song4_block1b:
	.byte NoteD +Oct4, 12, 12, 3 ; 1

	.byte NoteA +Oct3,  8, 14, 3 ; 2
	.byte NoteDb+Oct4,  4, 12, 3

	.byte NoteB +Oct3, 12, 14, 3 ; 3

	.byte NoteGb+Oct3, 12, 14, 3 ; 4

	.byte NoteG +Oct3,  4, 14, 3 ; 5
	.byte Rest, 2, 0, 0
	.byte NoteD +Oct4, 12, 12, 3

	.byte NoteBb+Oct3,  6, 14, 3 ; 6.5
	.byte 0

song4_block2b1:
	.byte NoteA +Oct3, 12, 14, 3

	.byte NoteA +Oct3, 4, 14, 3
	.byte NoteG +Oct4, 2, 12, 3
	.byte NoteG +Oct3, 2, 14, 3
	.byte NoteDb+Oct4, 4, 12, 3
	.byte 0

song4_block2b2:
	.byte NoteD +Oct4, 12, 10, 3
	.byte NoteD +Oct4, 12, 8, 3
	.byte 0

song4_blockListCh1:
	.word song4_block1t, song4_block1t, song4_block3t, song4_block4t
	.word song4_block5t, song4_block6t, song4_block7t, song4_block8t
	.word 0

song4_blockListCh2:
	.word song4_block1u, song4_block1u, song4_block3u, song4_block3u
	.word song4_block5u, song4_block6u, song4_block7u, song4_block8u
	.word 0

song4_blockListCh0:
	.word song4_measureRest, song4_measureRest, song4_measureRest, song4_measureRest
	.word song4_measureRest, song4_measureRest, song4_measureRest, song4_block8b1
	.word song4_block1b, song4_block2b1
	.word song4_block1b, song4_block2b2
	.word 0

; =================================================================
; ==== Song 8: Unnecessary Tension ====

song8_block1t:
	.byte NoteDb+Oct6, 1, 3, 15
	.byte NoteDb+Oct5, 1, 2, 15 
	.byte NoteDb+Oct5, 1, 2, 15 
	.byte NoteDb+Oct5, 1, 2, 15 
	.byte 0 	; note, duration, volume, envelope

song8_block2t:
	.byte NoteD+Oct6, 1, 3, 15
	.byte NoteD+Oct5, 1, 2, 15 
	.byte NoteD+Oct5, 1, 2, 15 
	.byte NoteD+Oct5, 1, 2, 15 
	.byte 0

song8_block1b:
	.byte NoteDb+Oct3, 3, 14, 3
	.byte NoteD+Oct3, 3, 13, 1 
	.byte Rest, 5, 0, 0 
	.byte NoteA+Oct3, 3, 11, 1 
	.byte NoteAb+Oct3, 2, 11, 1 

	.byte NoteDb+Oct3, 3, 14, 3
	.byte NoteD+Oct3, 3, 13, 1 
	.byte NoteGb+Oct3, 2, 11, 3 
	.byte NoteAb+Oct3, 7, 11, 1 
	.byte Rest, 1, 0, 0 
	.byte 0

song8_block2b:
	.byte NoteD+Oct3, 3, 14, 3
	.byte NoteEb+Oct3, 3, 13, 1 
	.byte Rest, 5, 0, 0 
	.byte NoteB+Oct3, 3, 11, 1 
	.byte NoteBb+Oct3, 2, 11, 1 

	.byte NoteD+Oct3, 3, 14, 3
	.byte NoteEb+Oct3, 3, 13, 1 
	.byte Rest, 2, 0, 0 
	.byte NoteBb+Oct3, 7, 11, 1 
	.byte Rest, 1, 0, 0 
	.byte 0

song8_block1p:
	.byte NoteD+Oct3, 2, 8, 15
	.byte NoteA+Oct6, 2, 2, 15
	.byte NoteA+Oct6, 2, 2, 15
	.byte NoteA+Oct6, 2, 2, 15
	.byte NoteA+Oct6, 3, 5, 1
	.byte NoteA+Oct6, 3, 2, 15
	.byte NoteA+Oct6, 2, 2, 15
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

song8_blockListP:
	.word song8_block1p
	.word song8_block1p
	.word 0

; Global variables

.segment "EXTZP": zeropage
ptrBlockList:
	.word 0
ptrBlock:
	.word 0
chStateOffset:
	.byte 0
vbiTmp1:
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
	jmp check_seq_end

execute_seq_step:
	sta seqTimer 				; reset seqTimer = seqStepDur

	lda #chSize*4
	sta chStateOffset

step_seq_loop:
	sec 
	sbc #chSize 
	sta chStateOffset

	jsr _stepChannelSequencer

	lda chStateOffset
	bne step_seq_loop
	jmp envelope_ch0

check_seq_end:
	lda #chSize*4
	sta chStateOffset

check_seq_end_loop:
	sec 
	sbc #chSize 
	sta chStateOffset

	jsr _checkForSeqenceEnd

	lda chStateOffset
	bne check_seq_end_loop

envelope_ch0:
	ldx #0
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF1
	lda chCurLvl
	jsr _setSquarishWaveTable
	lda chCurLvl
	jsr _setWaveTableEnabled
	
envelope_ch1:
	ldx #chSize*1
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF2
	jsr _getPokeyAudCtrlValue
	sta AUDC2
	
envelope_ch2:
	ldx #chSize*2
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF3
	jsr _getPokeyAudCtrlValue
	sta AUDC3
	
envelope_ch3:
	ldx #chSize*3
	stx chStateOffset

	jsr _stepChannelEnvelope
	jsr _getPokeyAudFreqValue
	sta AUDF4
	jsr _getPokeyAudCtrlValue
	sta AUDC4
	
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

	bne get_note 			; if ptrBlock == NULL: return
	rts 

get_note:
	lda noteIndex,X	 		; offset = noteIndex * 4
	asl a					; sequencer note unit is 4 bytes
	asl a					; 
	tay						; use Y for offset from block start
	lda (ptrBlock),Y		; get noteNumber
	bne save_note_number	; if noteNumber == 0: return
	rts 

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


.proc _checkForSeqenceEnd
	ldx chStateOffset
	lda seqEnable,X			; if seqEnable != 0: get block list ptr
	bne get_block_list_ptr
	rts

get_block_list_ptr:
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

	bne get_note 			; if ptrBlock == NULL: 
	lda #0
	sta blockIndex,X		; blockIndex = 0
	sta noteIndex,X 		; noteIndex = 0
	rts 

get_note:
	lda noteIndex,X	 		; offset = noteIndex * 4
	asl a					; sequencer note unit is 4 bytes
	asl a					; 
	tay						; use Y for offset from block start
	lda (ptrBlock),Y		; get noteNumber
	beq jump_to_next_block	; if noteNumber == 0: jump to next block
	rts 

jump_to_next_block:
	inc blockIndex,X		; blockIndex += 1
	lda #0
	sta noteIndex,X			; noteIndex = 0
	jmp get_block_ptr
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
	cmp #3
	beq envelope_3
	cmp #4
	beq envelope_4
	cmp #15
	beq envelope_15
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

envelope_3: 				; == Soft pad envelope with sustain ==
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

envelope_4: 				; == Piano with sustain ==
	lda #1		 			; fast attack
	sta chAtkTime,X

	lda chSusLvl, X
	asl a
	sta chAtkRate,X

	lda #1
	sta chDecRate,X 		; fast decay and release
	sta chRelRate,X

	lda #0
	jsr _calculateSustainTimeMinusA
	rts

envelope_15: 				; == Percussion envelope, no sustain ==
	lda chSusLvl,X			; no attack
	sta chAtkRate,X

	lda #1
	sta chAtkTime,X 		; fast decay and release
	sta chDecRate,X
	sta chRelRate,X

	lda #1
	sta chSusTime,X
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
	jsr _vbiMultiplyAX

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

.proc _vbiMultiplyAX
	; returns A * X in A
	; for use in VBI only
	sta vbiTmp1 	; save original value in vbiTmp1
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
	adc vbiTmp1
continue:
	asl vbiTmp1
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
	cmp noteTableLength
	bcs get_note 			; if note >= noteTableLength: return 255
	lda #$FF
	rts

get_note:
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
waveTable:
	.byte 0, 0, 0, 0, 0, 0, 0, 0
waveLength = 8
waveIndex:
	.byte 0

.code
.proc _waveTableTimerInterrupt
	txa
	pha

	ldx waveIndex
	lda waveTable,X
	ora #$10
	sta AUDC1

	inx 
	cpx #waveLength
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

; .proc _setTriangleWaveTable
; 	; on entry: A = waveform level
; 	cmp #$0F
; 	bcc skip_limiter
; 	lda #$0F
; skip_limiter:
; 	sta waveTable+4
; 	
; 	lsr a
; 	sta waveTable+2
; 	sta waveTable+6
; 	
; 	lsr a 
; 	sta waveTable+1
; 	sta waveTable+7
; 	
; 	adc waveTable+2
; 	sta waveTable+3
; 	sta waveTable+5
; 
; 	lda #0
; 	sta waveTable
; 
; 	rts
; .endproc


.proc _setSquarishWaveTable
	; on entry: A = waveform level
	cmp #$0F
	bcc check_same_level
	lda #$0F
check_same_level:
	cmp waveTable+4
	beq return

	sta waveTable+3
	sta waveTable+4
	sta waveTable+5
	
	lsr a
	sta waveTable+2
	sta waveTable+6

	lda #0
	sta waveTable
	sta waveTable+1
	sta waveTable+7

return:
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
	;sta STIMER 			; poke nonzero value to restart timers
return:
	rts
.endproc


; void __fastcall__ noteOn(UInt8 note, UInt8 duration, UInt8 volume, UInt8 envelope, UInt8 noise, UInt8 channel);
.export _noteOn			
.proc _noteOn		 			
	ldx #chSize  			; X = channel * chSize
	jsr _vbiMultiplyAX
	sta chStateOffset
	tax 

	ldy #4 					; get note
	lda (sp),Y
	sta chNote,X

	ldy #3 					; get duration
	lda (sp),Y
	sta noteStepsLeft,X

	ldy #2 					; get volume
	lda (sp),Y
	sta chSusLvl,X

	ldy #0 					; get noise
	lda (sp),Y
	sta chCtrlMask,X

	ldy #1 					; get envelope
	lda (sp),Y
	jsr _setEnvelope

	lda sp 					; pop parameters off stack
	clc
	adc #5
	sta sp
	lda sp+1
	adc #0
	sta sp+1

	rts
.endproc

; void __fastcall__ noteOff(UInt8 channel);
;.export _noteOff	
;.proc _noteOff
;	; This will allow notes to use their natural release rate
;	ldx #chSize 		; A = channel * chSize
;	jsr _vbiMultiplyAX
;	tax
;
;	lda #0
;	sta chSusTime,X
;	sta chAtkTime,X
;
;	rts
;.endproc

; void __fastcall__ startSong(UInt8 song);
.export _startSong 
.proc _startSong
	tay 					; save song parameter in Y

	lda #0 					; disable wave table
	jsr _setWaveTableEnabled

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
	cpy #4
	beq play_song_4
	cpy #8
	beq play_song_8
	jmp default

play_song_4:
	lda #7
	sta seqStepDur

	lda #<song4_blockListCh0
	sta blockListPtr+chSize*0
	lda #>song4_blockListCh0
	sta blockListPtr+chSize*0+1

	lda #<song4_blockListCh1
	sta blockListPtr+chSize*1
	lda #>song4_blockListCh1
	sta blockListPtr+chSize*1+1

	lda #<song4_blockListCh2
	sta blockListPtr+chSize*2
	lda #>song4_blockListCh2
	sta blockListPtr+chSize*2+1

	lda #1 						; enable channels that are used
	sta seqEnable+chSize*0
	sta seqEnable+chSize*1
	sta seqEnable+chSize*2
	rts

play_song_8:
	lda #5
	sta seqStepDur

	lda #<song8_blockListB
	sta blockListPtr+chSize*0
	lda #>song8_blockListB
	sta blockListPtr+chSize*0+1

	lda #<song8_blockListT
	sta blockListPtr+chSize*1
	lda #>song8_blockListT
	sta blockListPtr+chSize*1+1

	lda #<song8_blockListP
	sta blockListPtr+chSize*2
	lda #>song8_blockListP
	sta blockListPtr+chSize*2+1
	lda #$80
	sta chCtrlMask+chSize*2

	lda #1 						; enable channels that are used
	sta seqEnable+chSize*0
	sta seqEnable+chSize*1
	sta seqEnable+chSize*2
	rts

default:
	lda #5
	sta seqStepDur 				; medium tempo

	lda #<song0_blockListT
	sta blockListPtr+chSize*1
	lda #>song0_blockListT
	sta blockListPtr+chSize*1+1

	lda #<song0_blockListB
	sta blockListPtr+chSize*2
	lda #>song0_blockListB
	sta blockListPtr+chSize*2+1

	lda #1 						; enable channels that are used
	sta seqEnable+chSize*1
	sta seqEnable+chSize*2
	rts
.endproc

; void __fastcall__ stopSong(void);
.export _stopSong	
.proc _stopSong
	ldx #chSize*4
loop:
	txa 				; X -= chSize
	sec
	sbc #chSize
	tax

	lda #0
	sta seqEnable,X
	sta chSusTime,X
	sta chAtkTime,X
	sta noteStepsLeft,X
	sta noteIndex,X
	sta blockIndex,X

	cpx #0 				; while X > 0: loop
	bne loop

	lda #0 				; disable wave table
	jsr _setSquarishWaveTable
	lda #0 				
	jsr _setWaveTableEnabled

	rts
.endproc


.export _updateSoundSprites
.proc _updateSoundSprites
	HPOSM0 = $D004		; Missile 0 horizontal position

	ldx #chSize*4
	ldy #4
loop: 
	lda chCurLvl,X
	beq set_position
get_note:
	lda chNote,X		; get note
	cpx #0 				; channel 0 is two octaves lower than others
	beq shift_note
if_not_ch0:
	clc 
	adc #24 			; if not ch=0: shift 2 octaves up
shift_note:
	asl a				; else: note = note * 2 + 46
	clc
	adc #46 			; since notes start from 1, while 48 is the normal left edge
set_position:
	sta HPOSM0,Y 

	dey 
	txa 
	sec 
	sbc #chSize 
	tax 
	bcs loop
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
