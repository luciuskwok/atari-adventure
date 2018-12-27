// images.c

#include "images.h"
#include "atari_memmap.h"
#include "puff.h"
#include <string.h>


#define SCREEN_LENGTH (40 * 72)

// Tile Data

const UInt8 tileBitmaps[] = { 
	tCastle   , 0x00, 0x00, 0x22, 0x1C, 0x1C, 0x18, 0x22, 0x00, // Castle bkgnd
	tTown     , 0x00, 0x00, 0x28, 0x00, 0x28, 0x00, 0x00, 0x00, // Town bkgnd
	tVillage  , 0xFF, 0xD3, 0x89, 0xC9, 0xAC, 0x93, 0xCD, 0xF7, // Village bkgnd
	tMonument , 0xFF, 0xFF, 0xFF, 0xE7, 0xE3, 0xF1, 0xF8, 0xFC, // Monument bkgnd
	tCave     , 0xFF, 0xFF, 0xFF, 0xE7, 0xE7, 0xE7, 0xFF, 0xFF, // Cave bkgnd
	tPlains   , 0xFF, 0xF7, 0xFF, 0xBF, 0xFD, 0xFF, 0xEF, 0xFF, // Plains
	tBridgeH  , 0x7E, 0x7E, 0x00, 0x7E, 0x7E, 0x00, 0x7E, 0x7E, // Bridge (east-west)
	tBridgeV  , 0x00, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0xDB, 0x00, // Bridge (north-south)
	tForest   , 0xFF, 0xF7, 0xE3, 0xF7, 0xC1, 0xF7, 0x80, 0xF7, // Forest
	tShallows , 0xFE, 0xFF, 0xFB, 0xFF, 0xEF, 0xFF, 0xBF, 0xFF, // Shallow water
	tWater    , 0xCE, 0xB5, 0x5D, 0x73, 0xCE, 0xBA, 0xAD, 0x73, // Deep water
	tSolid    , 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Desert
	tMountain , 0xF7, 0xE3, 0xC1, 0x80, 0xFF, 0xBF, 0x1F, 0x0E, // Mountains
	tPotion   , 0x3C, 0x18, 0x18, 0x2C, 0x5E, 0x7E, 0x3C, 0x00, // Health Potion
	tFang     , 0x04, 0x04, 0x0C, 0x0C, 0x1C, 0x7C, 0x78, 0x30, 
	tBrick    , 0xDF, 0xDF, 0xDF, 0x00, 0xFD, 0xFD, 0xFD, 0x00, // Dungeon tiles
	tFloor    , 0xFE, 0xFF, 0xBB, 0xFF, 0xEF, 0xFF, 0xBB, 0xFF, 
	tLadder   , 0x24, 0x3C, 0x24, 0x3C, 0x24, 0x3C, 0x24, 0x00,
	tChest    , 0x3C, 0xFF, 0x00, 0xE7, 0xFF, 0xFF, 0xFF, 0x00,
	tHouse1   , 0xC0, 0xDF, 0x9F, 0xBF, 0x3F, 0x7F, 0x7F, 0x00, // Town tiles
	tHouse2   , 0x07, 0xE3, 0xEB, 0xC9, 0xDD, 0x9C, 0xBE, 0x00, 
	tHouse3   , 0x7F, 0x7F, 0x7F, 0x4C, 0x4C, 0x7F, 0x7F, 0x00, 
	tHouse4   , 0x6D, 0x6D, 0x6D, 0x12, 0x6D, 0x6D, 0x6D, 0x00, 
	tHouseDoor, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 
	0x00 // null terminator
};

// Unused tile bitmaps
// 0x00, 0x00, 0x02, 0x04, 0x76, 0x54, 0x74, 0x00, // of (unused)
// 0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00, // up arrow (unused)
// tLevel    , 0x00, 0x00, 0x40, 0x40, 0x45, 0x45, 0x72, 0x00, // Lv (Level)
// tHP       , 0x00, 0x00, 0x57, 0x55, 0x77, 0x54, 0x54, 0x00, // HP


void initTileFont(UInt8 fontPage) {
	const UInt8 *romFont = (UInt8 *)0xE000;
	UInt8 *customFont = (UInt8 *) ((UInt16)fontPage * 256);
	UInt8 *tileFont = customFont + 512;
	UInt16 index;
	UInt8 tileIndex, bitmapIndex;

	//print8bitValue("Start Font: ", fontPage, 1, 5);
	
	// Copy character set from ROM to RAM, 128 characters.
	memcpy(customFont, romFont, 1024);

	// Blank out the tile with value 0 in the graphics character area
	memset(tileFont, 0, 8);
	
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

SInt8 drawImage(const UInt8 *data, UInt16 length) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt16 screenLen = SCREEN_LENGTH;
	return puff(screen, &screenLen, data, &length);
}

void clearRasterScreen(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	memset(screen, 0, SCREEN_LENGTH);
}

