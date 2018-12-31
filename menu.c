// menu.c

#include "menu.h"
#include "cursor.h"
#include "sprites.h"


// Globals
PointU8 menuOrigin;
UInt8 menuItemCount;
UInt8 menuItemSpacing;
UInt8 menuIsHorizontal;
UInt8 menuSelectedIndex;
UInt8 menuEscapeCursorEvent;
const DataBlock *menuCursorSprite;

MenuDidClickCallbackType menuClickCallback;
MenuSelectedIndexDidChangeCallbackType menuSelectedIndexCallback;
MenuDidEscapeCallbackType menuEscapeCallback;

// Cursor Sprites
const DataBlock smallHeartSprite = { 4, { 0x36, 0x7F, 0x3E, 0x08 } };
const DataBlock	mediumHeartSprite = { 6, { 0x66, 0xFF, 0xFF, 0x7E, 0x3C, 0x18 } };

static void setCursorPosition(UInt8 x, UInt8 y) {
	static UInt8 previousY = 0;
	const UInt8 topMargin = 14;

	// Remove old sprite data
	fillSprite(1, 0, topMargin + previousY, menuCursorSprite->length);
	
	// Draw sprite in new position
	drawSprite(menuCursorSprite, 1, topMargin + y);
	setSpriteOriginX(1, PM_LEFT_MARGIN - 8 + x);

	previousY = y;
}

void setMenuSelectedIndex(UInt8 index) {
	UInt8 x = menuOrigin.x;
	UInt8 y = menuOrigin.y;

	if (menuIsHorizontal) {
		x += menuItemSpacing * index;
	} else {
		y += menuItemSpacing * index;
	}

	setCursorPosition(x, y);

	menuSelectedIndex = index;
	if (menuSelectedIndexCallback) {
		menuSelectedIndexCallback(index);
	}
}

static SInt8 menuCursorHandler(UInt8 event) {
	UInt8 index = menuSelectedIndex;

	switch (event) {
		case CursorClick:
			if (menuClickCallback) {
				return menuClickCallback(index);
			}
			return MessageNone;
		case CursorLeft:
			if (menuIsHorizontal) {
				--index; 
			}			
			break;
		case CursorRight: 
			if (menuIsHorizontal) {
				++index; 
			}			
			break;		
		case CursorUp:
			if (!menuIsHorizontal) {
				--index; 
			}			
			break;
		case CursorDown: 
			if (!menuIsHorizontal) {
				++index; 
			}			
			break;		
	}

	if (index != menuSelectedIndex && index < menuItemCount) {
		setMenuSelectedIndex(index);
	} else if (menuEscapeCursorEvent != CursorNone && event == menuEscapeCursorEvent && menuEscapeCallback) {
		menuEscapeCallback();
	}

	return MessageNone;
}

void setMenuCursor(UInt8 cursor) {
	clearSpriteData(1);
	setCursorColorCycling(1);
	setSpriteWidth(1, 1);
	if (cursor == SmallHeartCursor) {
		menuCursorSprite = &smallHeartSprite;
	} else {
		menuCursorSprite = &mediumHeartSprite;
	}
}

void hideCursor(void) {
	setSpriteOriginX(1, 0);
}

void setMenuClickHandler(MenuDidClickCallbackType cb) {
	menuClickCallback = cb;
}

void setMenuSelIndexHandler(MenuSelectedIndexDidChangeCallbackType cb) {
	menuSelectedIndexCallback = cb;
}

void setMenuEscapeHandler(MenuDidEscapeCallbackType cb) {
	menuEscapeCallback = cb;
}

void initMenu(void) {
	menuIsHorizontal = 0;
	menuSelectedIndex = 0;
	menuEscapeCursorEvent = CursorNone;

	menuClickCallback = NULL;
	menuSelectedIndexCallback = NULL;
	menuEscapeCallback = NULL;
	setCursorEventHandler(menuCursorHandler);
}
