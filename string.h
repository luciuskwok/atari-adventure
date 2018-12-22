// string.h

#include "types.h"

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value);
void hexString(UInt8 *outString, UInt16 value);
UInt8 strlen(const UInt8 *s);
void appendString(UInt8 *ioString, const UInt8 *append);

UInt8 toAtascii(UInt8 c);

