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
extern void __fastcall__ battleViewDLI(void);
extern void __fastcall__ infoViewDLI(void);

// Globals
UInt8 *textWindow;
static UInt8 *smallTextWindow;


// Display List constants
#define dl_Interrupt (0x80)
#define dl_LMS (0x40)
#define dl_VScroll (0x20)
#define dl_HScroll (0x10)
#define dl_mapTileLine (dl_Interrupt | dl_HScroll | 7)
#define dl_rasterLine (13)
#define dl_textWindowLine (2)

#define SCREEN_ROW_BYTES (40)


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
	const UInt8 screenLSB = startPage;
	const UInt8 screenMSB = 128;
	UInt8 *displayList = (UInt8 *)(startPage * 256);
	const UInt8 *screen = displayList + 128;
	const UInt8 textBarChartLSB = 120;

	// Init globals
	smallTextWindow = (UInt8 *)(textPage * 256); // Using the unused 384 bytes below PMGraphics
	textWindow = smallTextWindow;

	// Update OS pointers
	POKEW (SAVMSC, (UInt16)screen);
	POKEW (SDLSTL, (UInt16)displayList);

	// Set up lines common to all display lists
	displayList[0] = DL_BLK8; 
	displayList[1] = DL_BLK8; 
	displayList[2] = DL_BLK4; // 3
}

// Screen Modes

static UInt8 writeDisplayListLines(UInt8 *dl, const UInt8 *screen, UInt8 mode, UInt8 count) {
	UInt8 i;

	dl[0] = mode | dl_LMS; 
	dl[1] = (UInt16)screen % 256; 
	dl[2] = (UInt16)screen / 256; // 6
	
	for (i=0; i<count; ++i) {
		dl[i+3] = mode;
	}
	return count + 2; // number of bytes written
}

static UInt8 writeDisplayListCustomTextLines(UInt8 *dl, UInt8 count) {
	UInt8 textPage = (UInt16)textWindow / 256;
	UInt8 x = 0;

	// Text window
	dl[x++] = dl_textWindowLine | dl_LMS;
	dl[x++] = 0;
	dl[x++] = textPage;

	while (--count > 0) {
		dl[x++] = dl_textWindowLine; 
	}

	return x;
}

static UInt8 writeDisplayListBarChartLines(UInt8 *dl, const UInt8 *screen) {
	const UInt8 screenLSB = (UInt16)screen % 256;
	const UInt8 screenMSB = (UInt16)screen / 256;
	UInt8 x = 0;
	UInt8 count;

	dl[x++] = DL_BLK1;
	for (count=0; count<3; ++count) {
		dl[x++] = dl_rasterLine | dl_LMS; // Bar chart uses 3 repeating rows
		dl[x++] = screenLSB;
		dl[x++] = screenMSB;
	}
	dl[x++] = DL_BLK1;
	return x;
}

static void writeDisplayListEnd(UInt8 *dl) {
	dl[0] = DL_JVB; // Vertical blank + jump to DL start
	dl[1] = PEEK(SDLSTL);
	dl[2] = PEEK(SDLSTL+1);
}

static void writeMapViewDisplayList(void) {
	UInt8 *dl = (UInt8 *)PEEKW(SDLSTL);
	const UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 x = 3;

	x += writeDisplayListLines(dl+3, screen, dl_mapTileLine, 9);  // 14
	dl[x++] = DL_BLK8;
	x += writeDisplayListCustomTextLines(dl+x, 3);
	x += writeDisplayListBarChartLines(dl+x, textWindow + (3 * SCREEN_ROW_BYTES));
	dl[x-1] |= dl_Interrupt; 

	// Party stats line
	dl[x++] = DL_BLK8;
	dl[x++] = dl_textWindowLine; 

	writeDisplayListEnd(dl+x);
}

static void writeStoryViewDisplayList(void) {
	UInt8 *dl = (UInt8 *)PEEKW(SDLSTL);
	const UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 x = 3;

	x += writeDisplayListLines(dl+3, screen, dl_rasterLine, 72);
	x += writeDisplayListLines(dl+x, textWindow, dl_textWindowLine, 7);

	writeDisplayListEnd(dl+x);
}

static void writeBattleViewDisplayList(void) {
	UInt8 *dl = (UInt8 *)PEEKW(SDLSTL);
	const UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	const UInt16 rasterHeight = 48;
	UInt8 x = 3;

	// Main raster area
	x += writeDisplayListLines(dl+3, screen, dl_rasterLine, rasterHeight); // 96
	dl[x-1] |= dl_Interrupt; 

	// Enemy HP bar
	x += writeDisplayListBarChartLines(dl+x, screen + (rasterHeight * SCREEN_ROW_BYTES)); // +8 = 104
	dl[x++] = DL_BLK4; // +4 = 180

	// Chara stats
	x += writeDisplayListCustomTextLines(dl+x, 7); // +56 = 164
	x += writeDisplayListBarChartLines(dl+x, textWindow + (7 * SCREEN_ROW_BYTES)); // +8 = 172
	dl[x-1] |= dl_Interrupt; 

	dl[x++] = DL_BLK8; // +8 = 180

	screen += (rasterHeight + 1) * SCREEN_ROW_BYTES;
	x += writeDisplayListLines(dl+x, screen, dl_rasterLine, 10); // +20 = 200

	writeDisplayListEnd(dl+x);
}

static void writeInfoViewDisplayList(void) {
	UInt8 *dl = (UInt8 *)PEEKW(SDLSTL);
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 x = 3;
	UInt8 i;

	// Move text window
	textWindow = screen + (24 * SCREEN_ROW_BYTES);

	x += writeDisplayListLines(dl+3, screen, dl_rasterLine, 24);
	dl[x-1] |= dl_Interrupt; // 48

	dl[x++] = dl_textWindowLine | dl_Interrupt;
	dl[x++] = DL_BLK2; // + 10: 58

	for (i=0; i<12; ++i) {
		dl[x++] = dl_textWindowLine;
	}
	dl[x-1] |= dl_Interrupt; // + 12*8 = +96: 154

	dl[x++] = DL_BLK4; // +4: 158

	dl[x++] = dl_textWindowLine | dl_Interrupt;
	dl[x++] = DL_BLK2; // +10: 168

	for (i=0; i<4; ++i) {
		dl[x++] = dl_textWindowLine;
	} // + 4*8 = +32: 200

	writeDisplayListEnd(dl+x);
}

void setScreenMode(UInt8 mode) {
	UInt8 dma = 0x2E; // standard playfield + missile DMA + player DMA + display list DMA
	UInt8 nmi = 0x40; // enable VBI
	void *dli = 0;

	textWindow = smallTextWindow;

	switch (mode) {
		case ScreenModeMap:
			writeMapViewDisplayList();
			nmi |= 0x80; // enable DLI
			dli = mapViewDLI;
			break;

		case ScreenModeDialog:
			writeStoryViewDisplayList();
			break;

		case ScreenModeBattle:
			writeBattleViewDisplayList();
			nmi |= 0x80; // enable DLI
			dli = battleViewDLI;
			break;

		case ScreenModeInfo:
			writeInfoViewDisplayList();
			nmi |= 0x80; // enable DLI
			dli = infoViewDLI;
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
	waitVsync(1);

	if (dli != 0) {
		POKEW (VDSLST, (UInt16)dli);
	}

	POKE (SDMCTL, dma);
	ANTIC.nmien = nmi;
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

void loadColorTable(const UInt8 *colors) {
	UInt8 *p = (UInt8*)(PCOLR0);
	UInt8 i;
	if (colors) {
		for (i=0; i<12; ++i) {
			p[i] = colors[i];
		}
	} else {
		memset(p, 0, 12);
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

// Drawing

void setPixel(UInt8 *screen, UInt8 x, UInt8 y, UInt8 value) {
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

// Timing

void waitVsync(UInt8 ticks) {
	*VB_TIMER = ticks;
	while (*VB_TIMER != 0) {} // Wait for VSYNC 
}


