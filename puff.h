// puff.h

#include "types.h"

/* 
Notes on using puff():
This version of Puff is not re-entrant, because it uses globals for state in order to get faster execution.
*/

SInt8 puff(UInt8 *dest, UInt16 *destLen, const UInt8 *source, UInt16 *sourceLen);


/* Debugging */
extern UInt16 __fastcall__ decode_asm(const struct huffman *h);
//extern UInt16 debugging_data[];

