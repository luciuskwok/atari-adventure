// graphics.c

/*  == Notes on Memory Usage ==
	
	In the config.cfg file, the reserved memory is set to 0x1020, giving us 4KB 
	of space below the 1 KB display area for a total of 5KB. The space between 
	APPMHI and MEMTOP should be ours to use. 

	RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values without BASIC.

	0xB400: 3 KB: Display list and screen memory, within 4 KB boundaries.
	0xB000: 1 KB: PMGraphics needs 640 bytes (double-line sprites), but the data area 
		doesn't start until 384 (0x180) bytes after PMBase, within 1KB boundaries.
	0xAC00: 1 KB: Custom character set needs 128 chars * 8 bytes = 1024 bytes.

	Screen memory is allocated:
	- Map View:
		- Display List: 32 bytes
		- Screen memory: 24x9 = 216 bytes
	- Story View:
		- Display List: 96 bytes
		- Screen memory: 40x72 = 2,880 bytes
	- Total: about 3,008 bytes if screen memory is shared between the 2 views.
	- Shared text window: 40x7 = 280 bytes. Goes into memory hole at start of PMGraphics.

	- Display list should not cross a 1KB boundary (0x0400)
	- Screen memory should not cross a 4KB boundary (0x1000)
*/

#include "graphics.h"
#include "atari_memmap.h"
#include "map_data.h"
#include "puff.h"
#include "sprites.h"
#include <atari.h>


// Globals
UInt8 *graphicsWindow;
UInt8 *textWindow;



// Transition Effects

static UInt8 applyFade(UInt8 color, UInt8 amount) {
	UInt8 lum = color & 0x0F;
	if (amount > lum) {
		color = 0;
	} else {
		color -= amount;
	}
	return color;
}

void fadeOutColorTable(UInt8 fadeOptions) {
	UInt8 *colors = (UInt8*)(PCOLR0);
	UInt8 count;
	UInt8 i;

	// Disable player color cycling to avoid conflicts
	setCursorColorCycling(0);

	for (count=0; count<15; ++count) {
		*VB_TIMER = 1;

		for (i=0; i<12; ++i) {
			colors[i] = applyFade(colors[i], 1);			
		}
		if (fadeOptions & FadeGradient) {
			for (i=0; i<72; ++i) {
				BG_COLOR[i] = applyFade(BG_COLOR[i], 1);				
			}
		}

		// Delay 
		while (*VB_TIMER) {
		}
	}
}

void fadeInColorTable(UInt8 fadeOptions, const UInt8 *colorTable) {
	UInt8 *colors = (UInt8*)(PCOLR0);
	SInt8 amount;
	UInt8 i;

	for (amount=15; amount>=0; --amount) {
		*VB_TIMER = 1;

		for (i=0; i<12; ++i) {
			colors[i] = applyFade(colorTable[i], amount);			
		}
		if (fadeOptions & FadeGradient) {
			for (i=0; i<72; ++i) {
				//BG_COLOR[i] = applyFade(BG_COLOR[i]);				
			}
		}

		// Delay 
		while (*VB_TIMER) {
		}
	}
}

// Color Table

/*
void setBackgroundGradient(const UInt8 *data) {
	// colors: array of { length, value } tuples, with NULL terminator
	// There should be enough for 73 color values. First one goes into VBI shadow register, and
	// the rest changes the color at the end of each of 72 raster rows.

	UInt8 bgIndex = 0xFF;
	UInt8 dataIndex = 0;
	UInt8 length, value;

	while (length = data[dataIndex++]) {
		value = data[dataIndex++];

		while (length > 0) {
			if (bgIndex == 0xFF) {
				// First color goes into shadow register, which the OS VBI uses.
				POKE(COLOR4, value);
				bgIndex = 0;
			} else {
				BG_COLOR[bgIndex++] = value;
			}
			--length;
		}
	}
}
*/

// Drawing

SInt8 drawImage(const DataBlock *image, UInt8 rowOffset, UInt8 rowCount) {
	UInt8 *screen = graphicsWindow;
	UInt16 screenLen = rowCount * SCREEN_ROW_BYTES;
	UInt16 dataLen = image->length;

	screen += rowOffset * SCREEN_ROW_BYTES;
	return puff(screen, screenLen, image->bytes, dataLen);
}

