// graphics.c


#include "graphics.h"
#include "atari_memmap.h"
#include "map_data.h"
#include "sprites.h"
#include <atari.h>

extern unsigned __fastcall__ inflatemem (char* dest, const char* source);

// Drawing

UInt16 drawImage(const DataBlock *image, UInt8 rowOffset) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	screen += rowOffset * SCREEN_ROW_BYTES;
	return inflatemem(screen, image->bytes);
}

