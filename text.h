// text.h

#include "types.h"

#ifndef TEXT_H
#define TEXT_H

void clearTextWindow(UInt8 lines);
void clearTextRect(RectU8 *rect);

void printAllCharaText(UInt8 y);
void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs);

void printString(const UInt8 *s, UInt8 x, UInt8 y);
void drawTextBox(const UInt8 *s, UInt8 x, UInt8 y, UInt8 width, UInt8 lineSpacing, SInt8 indent);

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value);
void hexString(UInt8 *outString, UInt8 length, UInt16 value);

void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y);

// From text_asm.h
extern void __fastcall__ stringConcat(UInt8 *dst, const UInt8 *src);
extern void __fastcall__ stringCopy(UInt8 *dst, const UInt8 *src);
extern UInt8 __fastcall__ stringLength(UInt8 *s);

#endif
