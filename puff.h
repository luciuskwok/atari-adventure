// puff.h

#include "types.h"

/* 

Notes on using puff():
This version of Puff is not re-entrant, because it uses globals for state in order to get faster execution.

Puff adds about 3,945 bytes to the executable file size.

*/

SInt16 puff(UInt8 *dest, UInt16 destLen, const UInt8 *source, UInt16 sourceLen);

