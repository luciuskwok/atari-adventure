// sprites.h

#include "types.h"

#define PM_LEFT_MARGIN (48)
#define PM_TOP_MARGIN (14)

extern UInt8 *spriteArea;

void initSprites(UInt8 page);

void setPlayerCursorVisible(UInt8 visible);
void setCursorColorCycling(UInt8 cycle);

void setTileSprite(UInt8 spriteIndex);
void drawSprite(const DataBlock *sprite, UInt8 player, UInt8 y);
void fillSprite(UInt8 player, UInt8 value, UInt8 offset, UInt8 length);

void setSpriteWidth(UInt8 player, UInt8 width);

void setSpriteOriginX(UInt8 player, UInt8 x);

void clearSprite(UInt8 player);
void hideSprites(void);
