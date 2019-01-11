; atari_memmap.asm

	RTCLOK_LSB = $14 		; LSB of internal clock
	LMARGN    = $52
	RMARGN 	  = $53
	ROWCRS    = $54 		; cursor row
	COLCRS    = $55 		; cursor column
	SAVMSC 	  = $58 		; Pointer to screen memory
	SAVADR    = $68 		; Temporary pointer for screen row
	RAMTOP 	  = $6A
	BUFSTR    = $6C
	BITMSK    = $6E
	SHFAMT    = $6F
	ROWINC    = $79
	COLINC    = $7A

	VDSLST    = $0200 		; Pointer to current display list handler
	SDMCTL    = $022F 		; Shadow register for ANTIC options
	SDLSTL    = $0230		; Pointer to display list
	LINBUF    = $0247

	TXTMSC    = $0294		; Text window pointer
	BOTSCR    = $02BF 		; Number of lines in text window

	PCOLR0    = $02C0		; Player 0 color
	CHBAS     = $02F4

	DMACTL    = $D400 		; ANTIC options
	HSCROL    = $D404 		; ANTIC horizontal scroll
	NMIEN     = $D40E 		; ANTIC NMI enable
