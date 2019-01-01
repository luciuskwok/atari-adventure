// text.h

#include "types.h"
#include "misc_asm.h"

#ifndef TEXT_H
#define TEXT_H

void clearTextWindow(UInt8 lines);
void clearTextRect(RectU8 *rect);

void drawHpBar(UInt8 x, UInt8 y, UInt8 hp, UInt8 maxHp);
void printCharaAtIndex(UInt8 index, UInt8 y, UInt8 clear);
void printAllCharaText(UInt8 y);
void printPartyStats(void);

void printString(const UInt8 *s, UInt8 x, UInt8 y);
void drawTextBox(const UInt8 *s, UInt8 x, UInt8 y, UInt8 width, UInt8 lineSpacing, SInt8 indent);

void uint8toString(UInt8 *outString, UInt8 value);
void uint16toString(UInt8 *outString, UInt16 value);
void sint32toString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value);
void hexString(UInt8 *outString, UInt8 length, UInt16 value);

void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y);

#endif
