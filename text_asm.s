; text_asm.s

.export _stringLength 		; extern UInt8 __fastcall__ stringLength(UInt8 *s);
.export _stringConcat 		; extern void __fastcall__ stringConcat(UInt8 *dst, const UInt8 *src);

.import 	pushax
.import		incsp2, incsp4
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4

.proc _stringLength
; uses ptr1
STR = ptr1
	sta STR
	stx STR+1
	ldy #0

loop:
	lda (STR),Y
	cmp #0
	beq return

	iny

while_loop:
	cpy #$FF
	bne loop

return:
	tya
	ldx #0
	rts
.endproc


.proc _stringConcat
; uses ptr2, ptr3
SRC = ptr2
	sta SRC
	stx SRC+1
DST = ptr3
	ldy #1				; MSB
	lda (sp),Y
	sta DST+1
	tax
	ldy #0				; LSB
	lda (sp),Y
	sta DST
	
	jsr _stringLength	; DST += stringLength(DST)
	clc
	adc DST
	sta DST
	lda DST+1
	adc #0
	sta DST+1

	ldy #0

loop:
	lda (SRC),Y
	sta (DST),Y
	cmp #0
	beq return

	iny

while_loop:
	cpy #$FF
	bne loop

return:
	jsr incsp2
	rts
.endproc
