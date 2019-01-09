; graphics_asm.s

.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4


; extern void __fastcall__ loadColorTable(UInt8 *colors);
.export _loadColorTable
.proc _loadColorTable
	; uses ptr1 
	PCOLR0 = $02C0		; Player 0 color

	sta ptr1 
	stx ptr1+1
	cpx #0
	beq zero_out

copy_colors:
	ldy #0 
loop_copy_colors:
	lda (ptr1),Y
	sta PCOLR0,Y
	iny 
	cpy #12 
	bne loop_copy_colors
	rts

zero_out:
	ldy #0 
loop_zero_out:
	lda #0
	sta PCOLR0,Y
	iny 
	cpy #12 
	bne loop_zero_out
	rts

.endproc
