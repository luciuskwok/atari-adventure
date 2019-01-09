// map_data.h

#include "types.h"

/*
Run-length encoded map format:
For each row, the first byte indicates length of the row, not including length byte.
High nybble indicates the tileChar to use.
Low nybble stores the number of repeated tiles in addition to the first one.
If low nybble is 0xF, the next byte contains an 8-bit value to add to the repeat.
*/


extern const UInt8 overworldRleMap[];
extern const SizeU8 overworldMapSize;
extern UInt8 overworldTileMap[];
extern const UInt8 overworldColorTable[];
extern const PointU8 overworldEntryPoint;

extern const UInt8 dungeonRleMap[];
extern const SizeU8 dungeonMapSize;
extern UInt8 dungeonTileMap[];
extern const UInt8 dungeonColorTable[];
extern const PointU8 dungeonEntryPoint;

extern const UInt8 townRleMap[];
extern const SizeU8 townMapSize;
extern UInt8 townTileMap[];
extern const UInt8 townColorTable[];
extern const PointU8 townEntryPoint;

// Constants

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

	tPotion, // 17
	tFang,

	// Tiles with sprite overlays
	tCastle, // 19
	tTown,
	tVillage,
	tMonument,
	tCave, 
	tHouseDoor,

};


