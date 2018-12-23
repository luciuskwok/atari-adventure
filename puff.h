// puff.h

#include "types.h"

SInt8 puff(UInt8 *dest, UInt16 *destLen, const UInt8 *source, UInt16 *sourceLen);

/* Profiling data */
extern UInt16 profiling_bits_called;
extern UInt16 profiling_stored_called;
extern UInt16 profiling_decode_called;
extern UInt16 profiling_codes_called;
