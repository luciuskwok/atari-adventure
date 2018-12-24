// puff.h

#include "types.h"

/* 
Notes on using puff():
This version of Puff is not re-entrant, because it uses globals for state in order to get faster execution.
*/

SInt8 puff(UInt8 *dest, UInt16 *destLen, const UInt8 *source, UInt16 *sourceLen);

//extern struct puff_state; // used by puff_asm

/* Profiling data */
extern UInt16 profiling_checkpoint[];

