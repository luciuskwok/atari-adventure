// sprites.h

#include "types.h"

#define PM_LEFT_MARGIN (48)
#define PM_TOP_MARGIN (14)


void initSprites(UInt8 page);

void setPlayerCursorVisible(UInt8 visible);
void setPlayerCursorColorCycling(UInt8 cycle);

void setTileOverlaySprite(const UInt8 *sprite, UInt8 column, UInt8 row);
void drawSprite(const UInt8 *sprite, UInt8 length, UInt8 player, UInt8 y);
void setSpriteHorizontalPosition(UInt8 player, UInt8 x);
void setMegaSprite(const UInt8 *sprite, const UInt8 length, const PointU8 *position, UInt8 magnification);

void clearSpriteData(UInt8 player);
void hideSprites(void);

// Sprite Data
extern const UInt8 cursorSprite1[];
extern const UInt8 cursorSprite2[];
extern const UInt8 tileSprites[];

extern const UInt8 temFaceSpriteHeight;
extern const UInt8 temFaceSprite[];

extern const UInt8 selectionCursorSpriteHeight;
extern const UInt8 selectionCursorSprite[];

