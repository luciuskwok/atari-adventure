// graphics.c


#include "graphics.h"
#include "atari_memmap.h"
#include "map_data.h"
#include "puff.h"
#include "sprites.h"
#include <atari.h>

// Drawing

SInt8 drawImage(const DataBlock *image, UInt8 rowOffset, UInt8 rowCount) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 screenLen = rowCount * SCREEN_ROW_BYTES;
	UInt16 dataLen = image->length;

	screen += rowOffset * SCREEN_ROW_BYTES;
	return puff(screen, screenLen, image->bytes, dataLen);
}

