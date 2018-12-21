// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

// Globals
extern UInt8 *textWindow; // Pointer to screen memory for text window
extern UInt8 currentRawMap[];
extern UInt8 *currentTileMap;
extern SizeU8 currentMapSize;
extern UInt8 currentMapType;

// Functions
void initGraphics(void);
void initDisplayList(void);
void initFont(UInt8 fontPage);
void initSprites(void);

void clearMapScreen(void);
void loadMap(UInt8 mapType, UInt8 variation);
void loadColorTable(const UInt8 *colors);
void decodeRleMap(UInt8 *outMap, UInt16 mapLength, const UInt8 *inRleMap);

void layoutCurrentMap(UInt8 sightDistance);
void drawCurrentMap(PointU8 *center);
void drawSprite(const UInt8 *sprite, UInt8 spriteLength, UInt8 player, UInt8 y);
void drawSpriteTile(const UInt8 *sprite, UInt8 column, UInt8 row);

// Constants
enum MapTypes {
	OverworldMapType = 0,
	DungeonMapType = 1,
	TownMapType = 2,
};



#endif
