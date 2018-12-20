// text.h

#ifndef TEXT_H
#define TEXT_H

#include "types.h"

void printCharaStats(UInt8 player, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp);
void printPartyStats(UInt16 money, UInt16 potions, UInt16 fangs, UInt16 reputation);
void clearTextWindow(void);

void printColorString(const UInt8 *s, UInt8 color, UInt8 x, UInt8 y);
void printString(const UInt8 *s, UInt8 x, UInt8 y);
void printDebugInfo(const UInt8 *label, UInt16 value, UInt8 position);

void numberString(UInt8 *outString, UInt8 thousandsSeparator, UInt16 value);
void hexString(UInt8 *outString, UInt16 value);
UInt8 strlen(const UInt8 *s);
void appendString(UInt8 *ioString, const UInt8 *append);


#endif
