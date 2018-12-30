// menu.h

#include "types.h"

// Callbacks
typedef SInt8 (*MenuDidClickCallbackType)(UInt8 index);
typedef void (*MenuSelectedIndexDidChangeCallbackType)(UInt8 index);
typedef void (*MenuDidEscapeCallbackType)(void);

// Globals
extern PointU8 menuOrigin;
extern UInt8 menuItemCount;
extern UInt8 menuItemSpacing;
extern UInt8 menuIsHorizontal;
extern UInt8 menuSelectedIndex;
extern UInt8 menuEscapeCursorEvent;

void setMenuSelectedIndex(UInt8 index);

void registerMenuDidClickCallback(MenuDidClickCallbackType cb);
void registerMenuSelectedIndexDidChangeCallback(MenuSelectedIndexDidChangeCallbackType cb);
void registerMenuDidEscapeCallback(MenuDidEscapeCallbackType cb);

void setMenuCursor(const UInt8 *sprite, UInt8 height);
void initMenu(void);


