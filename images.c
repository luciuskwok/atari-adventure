// images.c

#include "images.h"
#include "atari_memmap.h"
#include "cursor.h"
#include "puff.h"
#include "text.h"
#include <string.h>
#include <stdio.h>


#define SCREEN_LENGTH (40 * 72)


void drawImage(const UInt8 *data, UInt16 length) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 screenLen = SCREEN_LENGTH;
	SInt8 result;

	result = puff(screen, &screenLen, data, &length);

	if (result) {
		UInt8 *s;
		sprintf(s, "puff() error:%c", result);
		printString(s, 1, 1);
		waitForAnyInput();
	}
}

void clearRasterScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	memset(screen, 0, SCREEN_LENGTH);
}

