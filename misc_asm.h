// misc_asm.h

#include "types.h"

extern void __fastcall__ stringConcat(UInt8 *dst, const UInt8 *src);
extern void __fastcall__ stringCopy(UInt8 *dst, const UInt8 *src);
extern UInt8 __fastcall__ stringLength(UInt8 *s);

extern void __fastcall__ decodeRunLenRange(UInt8 *outData, UInt8 start, UInt8 end, const UInt8 *runLenData);
