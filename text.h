// text.h

#ifndef TEXT_H
#define TEXT_H

#include "types.h"

void printCharaStats(UInt8 player, const char *name, UInt8 level, UInt8 hp);
void printPartyStats(UInt16 money, UInt16 potions, UInt16 fangs, UInt16 reputation);
void clearTextWindow(void);

void printColorString(const char *s, UInt8 color, UInt8 x, UInt8 y);
void printString(const char *s, UInt8 x, UInt8 y);
void printDebugInfo(const char *label, UInt16 value, UInt8 position);
void hexString(char *s, UInt16 x);
UInt8 strlen(const char *s);


#endif
