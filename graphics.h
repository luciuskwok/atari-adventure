// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

// Globals
extern UInt8 *textWindow; // Pointer to screen memory for text window
extern UInt8 currentRawMap[];
extern UInt8 *currentTileMap;
extern SizeU8 currentMapSize;

// Functions
void initGraphics(void);
void initDisplayList(void);
void initFont(UInt8 fontPage);
void initSprites(void);
void clearScreen(void);


void loadMap(UInt8 index);
void loadColorTable(const UInt8 colors[]);

void decodeRleMap(UInt8 *outMap, UInt16 mapLength, const UInt8 *inRleMap);

void drawCurrentMap(PointU8 *center, UInt8 sightDistance);
void drawSprite(const UInt8 *sprite, UInt8 spriteLength, UInt8 player, UInt8 y);
void drawSpriteTile(const UInt8 *sprite, UInt8 column, UInt8 row);

// Constants
enum MapEnum {
	OverworldMap = 0,
	DungeonMap = 1,
};



#endif
