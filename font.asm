; font.s


.segment "FONT"

.export customFontData
customFontData:
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $0C, $0C, $18, $18, $00, $30, $00, $00
	.byte $66, $66, $66, $00, $00, $00, $00, $00
	.byte $66, $FF, $66, $FF, $66, $00, $00, $00
	.byte $18, $3E, $60, $18, $06, $7C, $18, $00
	.byte $66, $0E, $1C, $38, $70, $66, $00, $00
	.byte $3C, $60, $36, $6F, $66, $3C, $00, $00
	.byte $18, $18, $18, $00, $00, $00, $00, $00
	.byte $06, $0C, $18, $18, $18, $0C, $06, $00
	.byte $60, $30, $18, $18, $18, $30, $60, $00
	.byte $00, $36, $1C, $7F, $1C, $36, $00, $00
	.byte $00, $0C, $0C, $3F, $0C, $0C, $00, $00
	.byte $00, $00, $00, $00, $30, $30, $60, $00
	.byte $00, $00, $00, $7E, $00, $00, $00, $00
	.byte $00, $00, $00, $00, $30, $30, $00, $00
	.byte $03, $06, $0C, $18, $30, $60, $C0, $80
	.byte $00, $3C, $66, $66, $66, $3C, $00, $00
	.byte $00, $18, $38, $18, $18, $7E, $00, $00
	.byte $00, $3C, $66, $0C, $30, $7E, $00, $00
	.byte $00, $7C, $06, $1C, $06, $7C, $00, $00
	.byte $00, $66, $66, $7E, $06, $06, $00, $00
	.byte $00, $7E, $60, $7C, $06, $7C, $00, $00
	.byte $00, $3C, $60, $7C, $66, $3C, $00, $00
	.byte $00, $7E, $06, $0C, $18, $30, $00, $00
	.byte $00, $3C, $66, $3C, $66, $3C, $00, $00
	.byte $00, $3C, $66, $3E, $06, $3C, $00, $00
	.byte $00, $00, $60, $00, $00, $60, $00, $00
	.byte $00, $00, $30, $00, $00, $30, $60, $00
	.byte $06, $0C, $18, $30, $18, $0C, $06, $00
	.byte $00, $00, $7E, $00, $00, $7E, $00, $00
	.byte $60, $30, $18, $0C, $18, $30, $60, $00
	.byte $3C, $66, $0C, $18, $00, $18, $00, $00
	.byte $7E, $C3, $CF, $DB, $CE, $C0, $7E, $00
	.byte $0E, $1E, $36, $66, $7E, $66, $00, $00
	.byte $7C, $66, $7C, $66, $66, $7C, $00, $00
	.byte $3C, $66, $60, $60, $66, $3C, $00, $00
	.byte $7C, $66, $66, $66, $66, $7C, $00, $00
	.byte $7E, $60, $78, $60, $60, $7E, $00, $00
	.byte $7E, $60, $78, $60, $60, $60, $00, $00
	.byte $3E, $60, $60, $66, $66, $3E, $00, $00
	.byte $66, $66, $7E, $66, $66, $66, $00, $00
	.byte $3C, $18, $18, $18, $18, $3C, $00, $00
	.byte $06, $06, $06, $06, $66, $3C, $00, $00
	.byte $66, $6C, $78, $78, $6C, $66, $00, $00
	.byte $60, $60, $60, $60, $60, $7E, $00, $00
	.byte $63, $77, $7F, $6B, $63, $63, $00, $00
	.byte $66, $66, $76, $6E, $66, $66, $00, $00
	.byte $3C, $66, $66, $66, $66, $3C, $00, $00
	.byte $7C, $66, $7C, $60, $60, $60, $00, $00
	.byte $3C, $66, $66, $66, $66, $38, $07, $00
	.byte $7C, $66, $7C, $78, $6C, $66, $00, $00
	.byte $3C, $60, $3C, $06, $06, $3C, $00, $00
	.byte $7E, $18, $18, $18, $18, $18, $00, $00
	.byte $66, $66, $66, $66, $66, $3C, $00, $00
	.byte $66, $66, $6C, $78, $70, $60, $00, $00
	.byte $63, $63, $63, $6B, $3E, $36, $00, $00
	.byte $66, $3C, $18, $18, $3C, $66, $00, $00
	.byte $66, $66, $3C, $18, $18, $18, $00, $00
	.byte $7E, $0E, $1C, $38, $70, $7E, $00, $00
	.byte $1E, $18, $18, $18, $18, $1E, $00, $00
	.byte $C0, $60, $30, $18, $0C, $06, $03, $01
	.byte $78, $18, $18, $18, $18, $78, $00, $00
	.byte $18, $3C, $66, $00, $00, $00, $00, $00
	.byte $00, $00, $00, $00, $00, $00, $FF, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF
	.byte $FF, $F7, $FF, $BF, $FD, $FF, $EF, $FF
	.byte $7E, $7E, $00, $7E, $7E, $00, $7E, $7E
	.byte $00, $DB, $DB, $DB, $DB, $DB, $DB, $00
	.byte $44, $EE, $EE, $44, $11, $BB, $BB, $11
	.byte $FE, $FF, $FB, $FF, $EF, $FF, $BF, $FF
	.byte $CE, $B5, $5D, $73, $CE, $BA, $AD, $73
	.byte $DF, $8F, $07, $FD, $F8, $70, $20, $FF
	.byte $DF, $DF, $DF, $00, $FD, $FD, $FD, $00
	.byte $FE, $FF, $BB, $FF, $EF, $FF, $BB, $FF
	.byte $24, $3C, $24, $3C, $24, $3C, $24, $00
	.byte $3C, $FF, $18, $E7, $FF, $FF, $FF, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $3C, $18, $18, $2C, $5E, $7E, $3C, $00
	.byte $04, $04, $0C, $0C, $1C, $7C, $78, $30
	.byte $E0, $EF, $DF, $DF, $BF, $BF, $7F, $00
	.byte $07, $F7, $EB, $EB, $DD, $DD, $BE, $00
	.byte $7F, $7F, $7F, $4C, $4C, $7F, $7F, $00
	.byte $6D, $6D, $6D, $12, $6D, $6D, $6D, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $00, $00, $00, $00, $00, $00, $00, $00
	.byte $00, $00, $22, $1C, $1C, $18, $22, $00
	.byte $00, $00, $28, $00, $28, $00, $00, $00
	.byte $FF, $D3, $89, $C9, $AC, $93, $CD, $F7
	.byte $FF, $FF, $FF, $E7, $E3, $F1, $F8, $FC
	.byte $FF, $FF, $E7, $C3, $C3, $C3, $FF, $FF
	.byte $80, $80, $80, $80, $80, $80, $80, $00
	.byte $30, $18, $0C, $00, $00, $00, $00, $00
	.byte $00, $00, $3E, $66, $6E, $36, $00, $00
	.byte $60, $60, $7C, $66, $66, $7C, $00, $00
	.byte $00, $00, $3C, $60, $60, $3C, $00, $00
	.byte $06, $06, $3E, $66, $66, $3E, $00, $00
	.byte $00, $00, $3C, $66, $78, $3E, $00, $00
	.byte $0E, $18, $3C, $18, $18, $18, $00, $00
	.byte $00, $00, $3E, $66, $66, $3E, $06, $3C
	.byte $60, $60, $7C, $66, $66, $66, $00, $00
	.byte $18, $00, $78, $18, $18, $18, $00, $00
	.byte $18, $00, $18, $18, $18, $18, $18, $70
	.byte $60, $60, $6C, $78, $78, $6C, $00, $00
	.byte $78, $18, $18, $18, $18, $18, $00, $00
	.byte $00, $00, $76, $7F, $6B, $63, $00, $00
	.byte $00, $00, $6C, $76, $66, $66, $00, $00
	.byte $00, $00, $3C, $66, $66, $3C, $00, $00
	.byte $00, $00, $7C, $66, $66, $7C, $60, $60
	.byte $00, $00, $3E, $66, $66, $3E, $06, $06
	.byte $00, $00, $36, $38, $30, $30, $00, $00
	.byte $00, $00, $3E, $70, $0E, $7C, $00, $00
	.byte $00, $18, $7E, $18, $18, $0C, $00, $00
	.byte $00, $00, $66, $66, $66, $3E, $00, $00
	.byte $00, $00, $66, $6C, $38, $30, $00, $00
	.byte $00, $00, $63, $6B, $7F, $36, $00, $00
	.byte $00, $00, $66, $3C, $3C, $66, $00, $00
	.byte $00, $00, $66, $66, $66, $3E, $06, $3C
	.byte $00, $00, $7E, $0C, $30, $7E, $00, $00
	.byte $06, $0C, $0C, $38, $0C, $0C, $06, $00
	.byte $18, $18, $18, $18, $18, $18, $18, $18
	.byte $60, $30, $30, $1C, $30, $30, $60, $00
	.byte $00, $00, $70, $DB, $0E, $00, $00, $00
	.byte $00, $08, $18, $38, $18, $08, $00, $00
fontEnd: 

;fontPage2: ; custom graphic tiles
;	.byte $00, $00, $00, $00, $00, $00, $00, $00 ; Blank
;	.byte $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF ; Solid / Desert
;
;	.byte $FF, $F7, $FF, $BF, $FD, $FF, $EF, $FF ; Plains
;	.byte $7E, $7E, $00, $7E, $7E, $00, $7E, $7E ; BridgeH
;	.byte $00, $DB, $DB, $DB, $DB, $DB, $DB, $00 ; BridgeV
;	.byte $FF, $F7, $E3, $F7, $C1, $F7, $80, $F7 ; Forest
;	.byte $FE, $FF, $FB, $FF, $EF, $FF, $BF, $FF ; Shallows
;	.byte $CE, $B5, $5D, $73, $CE, $BA, $AD, $73 ; Water
;	.byte $F7, $E3, $C1, $80, $FF, $BF, $1F, $0E ; Mountains
;
;	.byte $DF, $DF, $DF, $00, $FD, $FD, $FD, $00 ; Brick
;	.byte $FE, $FF, $BB, $FF, $EF, $FF, $BB, $FF ; Floor
;	.byte $24, $3C, $24, $3C, $24, $3C, $24, $00 ; Ladder
;	.byte $3C, $FF, $00, $E7, $FF, $FF, $FF, $00 ; Chest
;	.byte $C0, $DF, $9F, $BF, $3F, $7F, $7F, $00 ; House1
;	.byte $07, $E3, $EB, $C9, $DD, $9C, $BE, $00 ; House2
;	.byte $7F, $7F, $7F, $4C, $4C, $7F, $7F, $00 ; House3
;	.byte $6D, $6D, $6D, $12, $6D, $6D, $6D, $00 ; House4
;
;	.byte $3C, $18, $18, $2C, $5E, $7E, $3C, $00 ; Potion
;	.byte $04, $04, $0C, $0C, $1C, $7C, $78, $30 ; Fang
;
;	.byte $00, $00, $22, $1C, $1C, $18, $22, $00 ; Castle
;	.byte $00, $00, $28, $00, $28, $00, $00, $00 ; Town
;	.byte $FF, $D3, $89, $C9, $AC, $93, $CD, $F7 ; Village
;	.byte $FF, $FF, $FF, $E7, $E3, $F1, $F8, $FC ; Monument
;	.byte $FF, $FF, $FF, $E7, $E7, $E7, $FF, $FF ; Cave
;	.byte $80, $80, $80, $80, $80, $80, $80, $00 ; HouseDoor
;
;	customTilesLength = 26*8


.segment "FONT_HEAD"
	.word customFontData
	.word fontEnd-1
