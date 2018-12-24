// puff.h

#include "types.h"

/* 
Notes on using puff():
This version of Puff is not re-entrant, because it uses globals for state in order to get faster execution.
*/

SInt8 puff(UInt8 *dest, UInt16 *destLen, const UInt8 *source, UInt16 *sourceLen);

extern UInt16 __fastcall__ bits_asm(UInt8 count);

/* Debugging data */
//extern UInt16 debugging_data[];

