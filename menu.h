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

enum MenuCursors {
	SmallHeartCursor, MediumHeartCursor
};
void setMenuCursor(UInt8 cursor);
void hideCursor(void);

void setMenuClickHandler(MenuDidClickCallbackType cb);
void setMenuSelIndexHandler(MenuSelectedIndexDidChangeCallbackType cb);
void setMenuEscapeHandler(MenuDidEscapeCallbackType cb);

void initMenu(void);


