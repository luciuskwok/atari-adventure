// map.h

#ifndef MAP_H
#define MAP_H

#include "types.h"

// Globals
extern UInt8 currentMapType;
extern UInt8 mapShipType;
extern UInt8 mapLampStrength;
extern UInt8 mapSightDistance;
extern PointU8 mapOverworldLocation;
extern PointU8 mapCurrentLocation;


// Constants
enum MapTypes {
	OverworldMapType = 0,
	DungeonMapType = 1,
	TownMapType = 2,
};

// extern const UInt8 *currentRunLenMap;
// extern const UInt8 *currentTileMap;
// extern SizeU8 currentMapSize;
// extern UInt8 currentMapType;

// Map Movement
void transitionToMap(UInt8 mapType, UInt8 shouldFadeOut, UInt8 shouldFadeIn);
void exitToOverworld(void);
void enterDungeon(void);
void enterTown(void);
UInt8 canMoveTo(PointU8 *pt);
SInt8 mapCursorHandler(UInt8 event);

// Map Info
UInt8 mapTileAt(PointU8 *pt);
PointU8 mapEntryPoint(UInt8 mapType);

// Map Drawing
void loadMap(UInt8 mapType, UInt8 sightDistance, PointU8 *location);
const UInt8 *colorTableForMap(UInt8 mapType);
void clearMapScreen(void);
void layoutCurrentMap(UInt8 sightDistance);
void drawCurrentMap(PointU8 *center);



#endif
