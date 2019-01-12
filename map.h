// map.h

#ifndef MAP_H
#define MAP_H

#include "types.h"

// Globals
extern UInt8 currentMapType;
extern UInt8 playerShipType;
extern UInt8 playerLampStrength;
extern UInt8 playerSightDistance;
extern PointU8 playerOverworldLocation;
extern PointU8 playerLocation;

// Constants

enum MapTypes {
	OverworldMapType = 0,
	DungeonMapType = 1,
	TownMapType = 2,
};

// Map Drawing
const UInt8 *colorTableForMap(UInt8 mapType);
void clearMapScreen(void);
void layoutCurrentMap(UInt8 sightDistance);

// Map Movement
void transitionToMap(UInt8 mapType, UInt8 shouldFadeOut, UInt8 shouldFadeIn);
void exitToOverworld(void);
void enterDungeon(void);
void enterTown(void);
SInt8 mapCursorHandler(UInt8 event);

void initMap(void);

#endif
