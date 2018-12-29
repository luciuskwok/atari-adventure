// text.h

#include "types.h"

#ifndef TEXT_H
#define TEXT_H

void setTextBoxColors(void);

void clearTextWindow(void);
void printStatText(void);

void printString(const UInt8 *s, UInt8 x, UInt8 y);
void drawTextBox(const UInt8 *s, PointU8 *position, UInt8 width, UInt8 lineSpacing);

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value);
void hexString(UInt8 *outString, UInt8 length, UInt16 value);

UInt8 toAtascii(UInt8 c);

extern UInt8 __fastcall__ stringLength(UInt8 *s);
extern void __fastcall__ stringConcat(UInt8 *dst, UInt8 *src);

#endif
