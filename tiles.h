// tiles.h

#include "types.h"


#ifndef TILES_H
#define TILES_H



// Tile constants
// Add 0x40 to these values to get the character value.
// This puts them in the graphics/control character range in ATASCII.

// Map Tiles
enum TileValues {
	tSolid = 1,

	// Overworld
	tPlains,
	tBridgeH,
	tBridgeV,
	tForest, // also used in town
	tShallows,
	tWater,
	tMountain,

	// Dungeon/Town
	tBrick,
	tFloor,
	tLadder,
	tChest,
	tHouse1,
	tHouse2,
	tHouse3,
	tHouse4,

	// Tiles with sprite overlays
	tCastle = 24,
	tTown,
	tVillage,
	tMonument,
	tCave, 
	tHouseDoor,
};

// Text window
#define tPotion (0x3B)
#define tFang (0x3D)

// Bitmap Data
extern const UInt8 tileBitmaps[];
extern const UInt8 cursorSprite1[];
extern const UInt8 cursorSprite2[];
extern const UInt8 tileSprites[];
extern const UInt8 sansMegaSprite[];
extern const UInt8 sansMegaSpriteLength;

#endif
