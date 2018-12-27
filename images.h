// images.h

#include "types.h"

void initTileFont(UInt8 fontPage);
SInt8 drawImage(const UInt8 *data, UInt16 length);
void clearRasterScreen(void);


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

	tPotion = 0x3B,
	tFang = 0x3D,
};
