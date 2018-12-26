// puff.h

#include "types.h"

/* 
Notes on using puff():
This version of Puff is not re-entrant, because it uses globals for state in order to get faster execution.
*/

SInt16 puff(UInt8 *dest, UInt16 *destLen, const UInt8 *source, UInt16 *sourceLen);


/* Debugging */

int construct(struct huffman *h, const UInt8 *length, int n);
SInt16 codes(const struct huffman *lencode, const struct huffman *distcode);
SInt16 fixed(void);
SInt16 dynamic(void);
SInt16 stored(void);

