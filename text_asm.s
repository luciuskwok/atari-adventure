; text_asm.s

.export _stringConcat 		; extern void __fastcall__ stringConcat(UInt8 *dst, const UInt8 *src);
.export _stringCopy 		; extern void __fastcall__ stringCopy(UInt8 *dst, const UInt8 *src);
.export _stringLength 		; extern UInt8 __fastcall__ stringLength(UInt8 *s);

.import 	pushax
.import		incsp2, incsp4
.importzp 	sp, sreg
.importzp 	tmp1, tmp2, tmp3, tmp4, ptr1, ptr2, ptr3, ptr4



.proc _stringConcat
; uses ptr1, ptr2
; ptr1 is _stringLength parameter.
SRC = ptr2
	sta SRC
	stx SRC+1
DST = ptr1
	ldy #1				; MSB
	lda (sp),Y
	sta DST+1
	ldy #0				; LSB
	lda (sp),Y
	sta DST

	jsr _stringLengthInternal ; DST += stringLength(DST)
	clc
	adc DST
	sta DST
	lda #0
	adc DST+1
	sta DST+1

	jsr _stringCopyInternal
	jsr incsp2
	rts
.endproc

.proc _stringCopy
; uses ptr1, ptr2
SRC = ptr2
	stx SRC+1
	sta SRC
DST = ptr1
	ldy #1				; MSB
	lda (sp),Y
	sta DST+1
	ldy #0				; LSB
	lda (sp),Y
	sta DST

	jsr _stringCopyInternal
	jsr incsp2
	rts
.endproc

.proc _stringCopyInternal
; Copy null-terminated string from ptr2 to ptr1.
; On input, ptr1 is destination, ptr2 is source.
	ldy #0
loop:
	lda (ptr2),Y
	sta (ptr1),Y
	iny
	beq return
	cmp #0
	bne loop
return:
	rts
.endproc

.proc _stringLength
; uses ptr1
STR = ptr1
	sta STR
	stx STR+1
	jsr _stringLengthInternal
	rts
.endproc

.proc _stringLengthInternal
; On input, ptr1 points to string
STR = ptr1
	ldy #0
loop:
	lda (STR),Y
	beq return
	iny
	bne loop
return:
	tya
	ldx #0
	rts
.endproc
