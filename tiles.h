// tiles.h

#ifndef TILES_H
#define TILES_H

#include "types.h"


// Color constants
#define DesertColor (0x00)
#define PlainsColor (0x40)
#define ForestColor (0x80)
#define WaterColor  (0xC0)
#define DungeonWallColor  (0x00)
#define DungeonFloorColor (0x40)
#define DungeonExitColor  (0x80)
#define DungeonChestColor (0xC0)
#define TownGrayColor  (0x00)
#define TownRedColor   (0x40)
#define TownGreenColor (0x80)
#define TownBlueColor  (0xC0)

// Tile constants
// Add 0x40 to these values to get the character value.
// This puts them in the graphics/control character range in ATASCII.

// Overworld tiles
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

// Overworld Map
extern const UInt8 overworldRleMap[];
extern const SizeU8 overworldMapSize;
extern UInt8 overworldTileMap[];
extern const UInt8 overworldColorTable[];
extern const PointU8 overworldEntryPoint;

// Dungeon Map
extern const UInt8 dungeonRleMap[];
extern const SizeU8 dungeonMapSize;
extern UInt8 dungeonTileMap[];
extern const UInt8 dungeonColorTable[];
extern const PointU8 dungeonEntryPoint;

// Town Map
extern const UInt8 townRleMap[];
extern const SizeU8 townMapSize;
extern UInt8 townTileMap[];
extern const UInt8 townColorTable[];
extern const PointU8 townEntryPoint;

#endif
