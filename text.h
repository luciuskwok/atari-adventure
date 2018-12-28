// text.h

#include "types.h"

#ifndef TEXT_H
#define TEXT_H

void printCharaStats(UInt8 player, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp);
void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs, SInt16 reputation);
void clearTextWindow(void);
void setTextWindowColorTheme(UInt8 theme);

void printString(const UInt8 *s, UInt8 x, UInt8 y);
void drawTextBox(const UInt8 *s, PointU8 *position, UInt8 width);

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value);
void hexString(UInt8 *outString, UInt8 length, UInt16 value);

UInt8 toAtascii(UInt8 c);

#endif
