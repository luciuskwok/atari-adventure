// graphics.c

/*
== Notes on Memory Usage ==
In the config.cfg file, the reserved memory is set to 0x1020, giving us 4KB 
of space below the 1 KB display area for a total of 5KB. The space between 
APPMHI and MEMTOP should be ours to use. 

RAMTOP: 0xC0 without BASIC, 0xA0 with BASIC. Below are values with BASIC.

0x9C00: 1 KB: PMGraphics needs 640 bytes (double-line sprites), but the data area 
	doesn't start until 384 (0x180) bytes after PMBase, which must be on 1KB boundary.
0x9000: 3 KB: Display list and screen memory, which must be on 4 KB boundary.
0x8C00: 1 KB: Custom character set needs 128 chars * 8 bytes = 1024 bytes.

Screen memory is allocated:
- Map View:
	- Display List: 32 bytes
	- Screen memory: 24x9 = 216 bytes
- Story View:
	- Display List: 96 bytes
	- Screen memory: 40x72 = 2,880 bytes
- Total: about 3,008 bytes if screen memory is shared between the 2 views.
- Shared text window: 40x6 = 240 bytes. Goes into memory hole at start of PMGraphics.

- Display list should not cross a 1KB boundary (0x0400)
- Screen memory should not cross a 4KB boundary (0x1000)

*/


#include "graphics.h"
#include "atari_memmap.h"
#include "image_data.h"
#include "map.h"
#include "sprites.h"
#include "text.h"
#include <atari.h>


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

	initVBI(immediateUserVBI); // Safe value: 0xE45F
	
	// Set color table to all black and change the display list
	POKE (SDMCTL, 0x2E); // standard playfield + missile DMA + player DMA + display list DMA
	initDisplayList(ramtop - 12, ramtop - 16);
	selectDisplayList(1);
	initSprites(ramtop - 16);
	initFont(ramtop - 20);
	
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

void selectDisplayList(UInt8 index) {
	UInt8 oldSdmctl = PEEK(SDMCTL);

	POKE (SDMCTL, 0); // turn off DMA and DLI before changing pointers
	ANTIC.nmien = 0x40;

	switch (index) {
	case 1: // Map view
		POKEW (SDLSTL, (UInt16)mapViewDisplayList);
		POKEW (VDSLST, (UInt16)mapViewDLI);
		break;
	case 2: // Story view
		POKEW (SDLSTL, (UInt16)storyViewDisplayList);
		POKEW (VDSLST, (UInt16)storyViewDLI);
		break;
	default: // No DL: keep everything off.
		return;
	}

	ANTIC.nmien = 0xC0; // enable both DLI and VBI
	POKE (SDMCTL, oldSdmctl);
}

void initFont(UInt8 fontPage) {
	const UInt8 *romFont = (UInt8 *)0xE000;
	UInt8 *customFont = (UInt8 *) ((UInt16)fontPage * 256);
	UInt8 *tileFont = customFont + 512;
	UInt16 index;
	UInt8 tileIndex, bitmapIndex;

	//print8bitValue("Start Font: ", fontPage, 1, 5);
	
	// Copy character set from ROM to RAM, 128 characters.
	for (index=0; index<1024; ++index) {
		customFont[index] = romFont[index];
	}

	// Blank out the tile with value 0 in the graphics character area
	for (index=0; index<8; ++index) {
		tileFont[index] = 0;
	}
	
	// Add our custom tiles into the graphics character area
	bitmapIndex = 0;
	while (1) {
		// Each tile bitmap has 9 bytes. First byte indicated which character it replaces, 
		// or nil for the end of the data.
		tileIndex = tileBitmaps[bitmapIndex * 9];
		if (tileIndex == 0) {
			break;
		}
		for (index=0; index<8; ++index) { 
			tileFont[tileIndex * 8 + index] = tileBitmaps[bitmapIndex * 9 + index + 1];
		}
		++bitmapIndex;
	}
	
	// Set CHBAS to point to the graphics character set area.
	// This lets the  map tiles show in the color map part of the screen.
	// It seems that the text window area truncates the value to a multple of 4, 
	// neatly allowing for regular characters there.
	POKE(CHBAS, fontPage + 2);
}

// Color Table

void loadColorTable(const UInt8 *colors) {
	UInt8 *p = (UInt8*)(PCOLR0);
	UInt8 i;
	for (i=0; i<9; ++i) {
		p[i] = colors[i];
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


