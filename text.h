// text.h

#include "types.h"

#ifndef TEXT_H
#define TEXT_H

#define SET_TXT_ORIGIN(x, y)  *(UInt8*)0x52=(x);*(UInt8*)0x55=(x);*(UInt8*)0x54=(y);

void printCharaAtIndex(UInt8 index);
void printAllCharaStats(UInt8 row);

void printPartyStats(void);
void drawTextBox(const UInt8 *s);

void eraseCharaBoxAtIndex(UInt8 index);

void stringConcat(UInt8 *dst, const UInt8 *src);
void stringCopy(UInt8 *dst, const UInt8 *src);
UInt8 stringLength(UInt8 *s);
UInt8 toAtascii(UInt8 c);

void printLine(const UInt8 *s);
void printStringAtXY(const UInt8 *s, UInt8 x, UInt8 y);

UInt8 uint8toString(UInt8 *outString, UInt8 value);
UInt8 uint16toString(UInt8 *outString, UInt16 value);
void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y);

#endif
