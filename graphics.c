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
#include <string.h>

extern void __fastcall__ initVBI(void);
extern void __fastcall__ mapViewDLI(void);
extern void __fastcall__ battleViewDLI(void);
extern void __fastcall__ infoViewDLI(void);

// Globals
UInt8 *graphicsWindow;
UInt8 *textWindow;

#define SCREEN_ROW_BYTES (40)


// Screen Modes

void setScreenModeOLD(UInt8 mode) {
	const UInt8 dma = 0x2E; // standard playfield + missile DMA + player DMA + display list DMA
	void *dli = 0;

	// Turn off screen
	POKE (SDMCTL, 0);
	ANTIC.nmien = 0x40;

	if (mode == ScreenModeInfo) {
		POKEW (SAVMSC, (UInt16)graphicsWindow);
	} else {
		POKEW (SAVMSC, (UInt16)textWindow);
	}

	switch (mode) {
		case ScreenModeMap:
			// initMapViewDisplay();
			dli = mapViewDLI;
			break;

		case ScreenModeDialog:
			// initStoryViewDisplay();
			break;

		case ScreenModeBattle:
			// initBattleViewDisplay();
			dli = battleViewDLI;
			break;

		case ScreenModeInfo:
			// initInfoViewDisplay();
			dli = infoViewDLI;
			break;

		case ScreenModeOff:
		default:
			return;
	}

	// Wait for vblank to prevent flicker
	waitVsync(1);

	if (dli != 0) {
		POKEW (VDSLST, (UInt16)dli);
		ANTIC.nmien = 0xC0; // enable DLI + VBI
	}
	POKE (SDMCTL, dma);
}

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

static void setPixel(UInt8 *screen, UInt8 x, UInt8 y, UInt8 value) {
	UInt8 shift = (3 - (x % 4)) * 2;
	UInt8 mask = 3 << shift;
	UInt8 c;

	screen += x / 4 + SCREEN_ROW_BYTES * y;
	c = *screen;
	c &= ~mask;

	value <<= shift;
	c |= value;

	*screen = c;
}

void drawBarChart(UInt8 *screen, UInt8 x, UInt8 y, UInt8 width, UInt8 filled) {
	UInt8 i, value;

	for (i=0; i<width; ++i) {
		value = (i >= filled) ? 1 : 2;
		setPixel(screen, x, y, value);
		++x;
	}
}

SInt8 drawImage(const DataBlock *image, UInt8 rowOffset, UInt8 rowCount) {
	UInt8 *screen = graphicsWindow;
	UInt16 screenLen = rowCount * SCREEN_ROW_BYTES;
	UInt16 dataLen = image->length;

	screen += rowOffset * SCREEN_ROW_BYTES;
	return puff(screen, screenLen, image->bytes, dataLen);
}

void clearGraphicsWindow(UInt8 rows) {
	memset(graphicsWindow, 0, SCREEN_ROW_BYTES * rows);
}

// Timing

void waitVsync(UInt8 ticks) {
	*VB_TIMER = ticks;
	while (*VB_TIMER != 0) {} // Wait for VSYNC 
}

// Init

static void initDisplayList(UInt8 startPage, UInt8 textPage) {
	const UInt8 screenLSB = startPage;
	const UInt8 screenMSB = 128;
	UInt8 *displayList = (UInt8 *)(startPage * 256);
	const UInt8 textBarChartLSB = 120;

	// Init globals
	textWindow = (UInt8 *)(textPage * 256); // Using the unused 384 bytes below PMGraphics
	graphicsWindow = displayList + 128;

	// Update OS pointers
	POKEW (SDLSTL, (UInt16)displayList);

	// Set up lines common to all display lists
	displayList[0] = DL_BLK8; 
	displayList[1] = DL_BLK8; 
	displayList[2] = DL_BLK4; // 3
}

void initGraphics(void) {
	UInt8 ramtop = PEEK(RAMTOP);

	// Turn off screen during init and leave it off for main to turn back on.
	POKE (SDMCTL, 0);

	initVBI(); // Safe value: 0xE45F
	
	// Set color table to all black
	loadColorTable(NULL);

	initDisplayList(ramtop - 12, ramtop - 16);
	initSprites(ramtop - 16);
	initFont(ramtop - 20);
	
	// == Use scrolling to center the odd number of tiles ==
	ANTIC.hscrol = 4;
}


