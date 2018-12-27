// image_data.h

#include "types.h"


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

// Tile Data
extern const UInt8 tileBitmaps[];
extern const UInt8 cursorSprite1[];
extern const UInt8 cursorSprite2[];
extern const UInt8 tileSprites[];
// extern const UInt8 sansMegaSpriteLength;
// extern const UInt8 sansMegaSprite[];

// Raster Data
extern const UInt16 testImageLength;
extern const UInt8 testImage[];

extern const UInt16 temShopImageLength;
extern const UInt8 temShopImage[];

extern const UInt8 temFaceSpriteHeight;
extern const UInt8 temFaceSprite[];

extern const UInt8 selectionCursorSpriteHeight;
extern const UInt8 selectionCursorSprite[];
