// text.h

#include "types.h"

#ifndef TEXT_H
#define TEXT_H

void clearTextWindow(UInt8 lines);

void printAllCharaText(UInt8 y);
void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs);

void printString(const UInt8 *s, UInt8 x, UInt8 y);
void drawTextBox(const UInt8 *s, PointU8 *position, UInt8 width, UInt8 lineSpacing, SInt8 indent);

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value);
void hexString(UInt8 *outString, UInt8 length, UInt16 value);

UInt8 toAtascii(UInt8 c);

extern UInt8 __fastcall__ stringLength(UInt8 *s);
extern void __fastcall__ stringConcat(UInt8 *dst, UInt8 *src);

#endif
