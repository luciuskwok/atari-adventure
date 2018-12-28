// graphics.c

/*
== Notes on Memory Usage ==
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
#include "images.h"
#include "sprites.h"
#include <atari.h>
#include <string.h>


extern void __fastcall__ initVBI(void *addr);
extern void __fastcall__ immediateUserVBI(void);
extern void __fastcall__ mapViewDLI(void);
extern void __fastcall__ storyViewDLI(void);



// Globals
UInt8 *textWindow;
UInt8 *mapViewDisplayList;
UInt8 *storyViewDisplayList;



// Init

void initGraphics(void) {
	UInt8 ramtop = PEEK(RAMTOP);

	// Turn off screen during init and leave it off for main to turn back on.
	POKE (SDMCTL, 0);

	initVBI(immediateUserVBI); // Safe value: 0xE45F
	
	// Set color table to all black
	loadColorTable(NULL);

	initDisplayList(ramtop - 12, ramtop - 16);
	initSprites(ramtop - 16);
	initTileFont(ramtop - 20);
	
	// == Use scrolling to center the odd number of tiles ==
	ANTIC.hscrol = 4;
}

void initDisplayList(UInt8 startPage, UInt8 textPage) {
	// Create two display lists, one for map view, and the other for story view.
	const UInt8 dl_Interrupt = 0x80;
	const UInt8 dl_LMS = 0x40;
	const UInt8 dl_VScroll = 0x20;
	const UInt8 dl_HScroll = 0x10;
	const UInt8 mapTileLine = (dl_Interrupt | dl_HScroll | 7);
	const UInt8 rasterLine = (dl_Interrupt | 13);
	const UInt8 textWindowLine = 2;
	UInt8 *screen = (UInt8 *)(startPage * 256 + 128);
	UInt8 x = 0;
	UInt8 i;


	// Allocate display memory usage
	mapViewDisplayList = (UInt8 *)(startPage * 256);
	storyViewDisplayList = mapViewDisplayList + 32;
	textWindow = (UInt8 *)(textPage * 256); // Using the unused memory area below PMGraphics

	// Update screen memory pointer
	POKEW (SAVMSC, (UInt16)screen);

	// == Map View DL ==
	mapViewDisplayList[x++] = DL_BLK8;
	mapViewDisplayList[x++] = DL_BLK8;
	mapViewDisplayList[x++] = DL_BLK4;

	mapViewDisplayList[x++] = mapTileLine | dl_LMS;
	mapViewDisplayList[x++] = (UInt16)screen % 256;
	mapViewDisplayList[x++] = (UInt16)screen / 256;
	
	for (i=1; i<9; ++i) { // 9 rows * 16 scanlines = 144 scanlines
		mapViewDisplayList[x++] = mapTileLine; // DLI on every tile row
	}
	
	//mapViewDisplayList[x++] = DL_BLK2; // 2 blank scanlines

	// Text window
	mapViewDisplayList[x++] = textWindowLine | dl_LMS;
	mapViewDisplayList[x++] = (UInt16)textWindow % 256;
	mapViewDisplayList[x++] = (UInt16)textWindow / 256;

	for (i=1; i<7; ++i) { // 7 rows of text
		mapViewDisplayList[x++] = textWindowLine; 
	}
	
	mapViewDisplayList[x++] = DL_JVB; // Vertical blank + jump to beginning of display list
	mapViewDisplayList[x++] = (UInt16)mapViewDisplayList % 256;
	mapViewDisplayList[x++] = (UInt16)mapViewDisplayList / 256;

	// == Story View DL ==
	x = 0;
	storyViewDisplayList[x++] = DL_BLK8;
	storyViewDisplayList[x++] = DL_BLK8;
	storyViewDisplayList[x++] = DL_BLK4;
	// OLD: 2 fewer scanlines here to allow 2 blank scanlines between raster image and text window

	storyViewDisplayList[x++] = rasterLine | dl_LMS;
	storyViewDisplayList[x++] = (UInt16)screen % 256;
	storyViewDisplayList[x++] = (UInt16)screen / 256;
	
	for (i=1; i<72; ++i) { // 72 rows * 2 scanlines = 144 scanlines
		storyViewDisplayList[x++] = rasterLine; // DLI on every tile row
	}
	
	// OLD: Needs blank scanline for DLI to change text window colors in time
	//storyViewDisplayList[x++] = DL_BLK2; 

	// Text window
	storyViewDisplayList[x++] = textWindowLine | dl_LMS;
	storyViewDisplayList[x++] = (UInt16)textWindow % 256;
	storyViewDisplayList[x++] = (UInt16)textWindow / 256;

	for (i=1; i<7; ++i) { // 7 rows of text = 48 scanlines
		storyViewDisplayList[x++] = textWindowLine; 
	}
	
	storyViewDisplayList[x++] = DL_JVB; // Vertical blank + jump to beginning of display list
	storyViewDisplayList[x++] = (UInt16)storyViewDisplayList % 256;
	storyViewDisplayList[x++] = (UInt16)storyViewDisplayList / 256;
}

// Transition Effects

UInt8 applyFade(UInt8 color, UInt8 amount) {
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
	setPlayerCursorColorCycling(0);

	for (count=0; count<15; ++count) {
		*VB_TIMER = 1;

		for (i=0; i<9; ++i) {
			colors[i] = applyFade(colors[i], 1);			
		}
		if (fadeOptions & FadeGradient) {
			for (i=0; i<72; ++i) {
				BG_COLOR[i] = applyFade(BG_COLOR[i], 1);				
			}
		}
		if (fadeOptions & FadeTextBox) {
			*TEXT_LUM = applyFade(*TEXT_LUM, 1);
			*TEXT_BG = applyFade(*TEXT_BG, 1);
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

		for (i=0; i<9; ++i) {
			colors[i] = applyFade(colorTable[i], amount);			
		}
		if (fadeOptions & FadeGradient) {
			for (i=0; i<72; ++i) {
				//BG_COLOR[i] = applyFade(BG_COLOR[i]);				
			}
		}
		if (fadeOptions & FadeTextBox) {
			// *TEXT_LUM = applyFade(*TEXT_LUM);
			// *TEXT_BG = applyFade(*TEXT_BG);
		}

		// Delay 
		while (*VB_TIMER) {
		}
	}
}

void setScreenMode(UInt8 mode) {
	UInt8 dma = 0x2E; // standard playfield + missile DMA + player DMA + display list DMA
	UInt8 nmi = 0x40; // enable VBI
	void *dl = 0;
	void *dli = 0;

	*VB_TIMER = 1;

	switch (mode) {
		case ScreenModeMap:
			dma = 0x2E; // various Antic DMA options
			nmi |= 0x80; // enable DLI
			dl = mapViewDisplayList;
			dli = mapViewDLI;
			break;

		case ScreenModeShop:
			dma = 0x2E; // various Antic DMA options
			dl = storyViewDisplayList;
			dli = storyViewDLI;
			break;

		case ScreenModeBattle:
			dma = 0x2E; // various Antic DMA options
			nmi |= 0x80; // enable DLI
			dl = storyViewDisplayList;
			dli = storyViewDLI;
			break;

		case ScreenModeOff:
		default:
			dma = 0;
			break;
	}

	// Turn off screen
	POKE (SDMCTL, 0);
	ANTIC.nmien = 0x40;

	// Wait for vblank to prevent flicker
	while (*VB_TIMER != 0) {}

	if (dl != 0) {
		POKEW (SDLSTL, (UInt16)dl);
	}
	if (dli != 0) {
		POKEW (VDSLST, (UInt16)dli);
	}

	POKE (SDMCTL, dma);
	ANTIC.nmien = nmi;
}

// Color Table

void loadColorTable(const UInt8 *colors) {
	UInt8 *p = (UInt8*)(PCOLR0);
	UInt8 i;
	if (colors) {
		for (i=0; i<9; ++i) {
			p[i] = colors[i];
		}
	} else {
		memset(p, 0, 9);
	}
}

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


