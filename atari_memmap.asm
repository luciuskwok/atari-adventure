; atari_memmap.asm
	
	; Zero page OS
	RTCLOK_LSB = $14 		; LSB of internal clock

	TMPCHR    = $50 		; Temporary register used by display handler
	HOLD1     = $51 		; Also temporary register
	LMARGN    = $52
	RMARGN 	  = $53
	ROWCRS    = $54 		; cursor row
	COLCRS    = $55 		; cursor column, 16-bit
	DINDEX    = $57			; Current screen/display mode
	OLDROW    = $5A			; Previous graphics cursor row
	OLDCOL    = $5B			; Previous graphics cursor column, 16-bit
	SAVMSC 	  = $58 		; Pointer to screen memory, 16-bit
	NEWROW    = $60			
	NEWCOL    = $61			; 16-bit
	ADRESS    = $64 		; Temporary pointer, 16-bit
	SAVADR    = $68 		; Temporary pointer for screen row, 16-bit
	RAMTOP 	  = $6A
	BUFSTR    = $6C
	BITMSK    = $6E
	SHFAMT    = $6F
	DELTAR    = $76
	DELTAC    = $77
	ROWINC    = $79
	COLINC    = $7A


	; OS page 2
	VDSLST    = $0200 		; Pointer to current display list handler
	SDMCTL    = $022F 		; Shadow register for ANTIC options
	SDLSTL    = $0230		; Pointer to display list

	TXTMSC    = $0294		; Text window pointer
	BOTSCR    = $02BF 		; Number of lines in text window

	PCOLR0    = $02C0		; Player 0 color
	PCOLR1    = $02C1		; Player 1 color
	PCOLR2    = $02C2		; Player 1 color
	COLOR0    = $02C4		; Field 0 color (pixel value 1)
	COLOR1    = $02C5		; Field 1 color (pixel value 2)
	COLOR2    = $02C6		; Field 2 color (pixel value 3)
	COLOR3    = $02C7		; Field 3 color
	COLOR4    = $02C8		; Background color (pixel value 0)
	TXTLUM    = $02C9		; Extra color: Text luminance
	TXTBKG    = $02CA		; Extra color: Text box background color
	COLOR7    = $02CB		; Extra color: Bar background color

	CHBAS     = $02F4

	; GTIA
	HPOSP3    = $D003		; Player 3 horizontal position
	HPOSM0    = $D004		; Missile 0 horizontal position
	HPOSM1    = $D005		; Missile 0 horizontal position
	HPOSM2    = $D006		; Missile 0 horizontal position
	HPOSM3    = $D007		; Missile 0 horizontal position
	SIZEP0    = $D008
	SIZEP1    = $D009
	SIZEP2    = $D00A
	SIZEP3    = $D00B
	COLPF0    = $D016		; pixel 1
	COLPF1    = $D017		; text luminance / pixel 2
	COLPF2    = $D018		; text background / pixel 3
	COLPF3    = $D019		; 
	COLPF4    = $D01A		; background / pixel 0

	; ANTIC
	DMACTL    = $D400 		; ANTIC options
	HSCROL    = $D404 		; ANTIC horizontal scroll
	CHBASE    = $D409		; Character set
	WSYNC     = $D40A
	VCOUNT    = $D40B		; vertical line counter
	NMIEN     = $D40E 		; ANTIC NMI enable

	; OS Vectors
	SYSVBV    = $E45F 		; exit immediate user VBI routine
	XITVBV    = $E462 		; exit deferred user VBI routine
