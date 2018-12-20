// graphics.h

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "types.h"

// Globals
extern UInt8 testMap[];
extern UInt8 *textWindow;

void initGraphics(void);
void initDisplayList(void);
void initFont(UInt8 fontPage);
void initSprites(void);

void decodeRleMap(UInt8 *outMap, UInt16 mapLength, const UInt8 *inRleMap);

void clearScreen(void);
void drawMap(const UInt8 *map, UInt8 mapWidth, UInt8 mapHeight, UInt8 centerX, UInt8 centerY);
void drawSprite(const UInt8 *sprite, UInt8 spriteLength, UInt8 player, UInt8 y);
void drawSpriteTile(const UInt8 *sprite, UInt8 column, UInt8 row);



#endif
