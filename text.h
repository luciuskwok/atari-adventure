// text.h

#include "types.h"

#ifndef TEXT_H
#define TEXT_H

void printCharaStats(UInt8 player, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp);
void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs, SInt16 reputation);
void clearTextWindow(void);
void setTextWindowColorTheme(UInt8 theme);

void printString(const UInt8 *s, UInt8 x, UInt8 y);
void printStringWithLayout(const UInt8 *s, UInt8 top, UInt8 firstIndent, UInt8 leftMargin, UInt8 rightMargin);
void printDecimal16bitValue(const UInt8 *label, SInt16 value, UInt8 x, UInt8 y);
void printHex16bitValue(const UInt8 *label, UInt16 value, UInt8 x, UInt8 y);
void printHex8bitValue(const UInt8 *label, UInt8 value, UInt8 x, UInt8 y);

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value);
void hexString(UInt8 *outString, UInt8 length, UInt16 value);

UInt8 strlen(const UInt8 *s);
void appendString(UInt8 *ioString, const UInt8 *append);

UInt8 toAtascii(UInt8 c);

#endif
