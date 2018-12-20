// text.h

#ifndef TEXT_H
#define TEXT_H

#include "types.h"

void printString(const char *s, UInt8 color, UInt8 x, UInt8 y);
void printDebugInfo(const char *label, UInt16 value, UInt8 position);
void hexString(char *s, UInt16 x);
UInt8 strlen(const char *s);


#endif
