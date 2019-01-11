// text.h

#include "types.h"

#ifndef TEXT_H
#define TEXT_H

#define SET_TXT_ORIGIN(x, y)  *(UInt8*)0x52=(x);*(UInt8*)0x55=(x);*(UInt8*)0x54=(y);

void printCharaAtIndex(UInt8 index);
void printAllCharaText(UInt8 y);

extern void printPartyStats(void);
extern void drawTextBox(const UInt8 *s);

extern void eraseCharaBoxAtIndex(UInt8 index);

extern void stringConcat(UInt8 *dst, const UInt8 *src);
extern void stringCopy(UInt8 *dst, const UInt8 *src);
extern UInt8 stringLength(UInt8 *s);
extern UInt8 toAtascii(UInt8 c);

extern void printLine(const UInt8 *s);
extern void printStringAtXY(const UInt8 *s, UInt8 x, UInt8 y);

extern UInt8 uint8toString(UInt8 *outString, UInt8 value);
extern UInt8 uint16toString(UInt8 *outString, UInt16 value);
extern void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y);

#endif
