; map_.asm

.include "atari_memmap.asm"


; void fillMapRow(UInt8 c);
.export _fillMapRow
.proc _fillMapRow 
	; Fills tiles starting at SAVADR to NEWCOL width with A
	ldy #0 
	loop: 
		sta (SAVADR),Y
		iny 
		cpy NEWCOL
		bne loop
	rts
.endproc
