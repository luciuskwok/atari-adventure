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

MenuDidClickCallbackType menuClickCallback;
MenuSelectedIndexDidChangeCallbackType menuSelectedIndexCallback;
MenuDidEscapeCallbackType menuEscapeCallback;


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

void registerMenuDidClickCallback(MenuDidClickCallbackType cb) {
	menuClickCallback = cb;
}

void registerMenuSelectedIndexDidChangeCallback(MenuSelectedIndexDidChangeCallbackType cb) {
	menuSelectedIndexCallback = cb;
}

void registerMenuDidEscapeCallback(MenuDidEscapeCallbackType cb) {
	menuEscapeCallback = cb;
}

void setMenuCursor(const UInt8 *sprite, UInt8 height) {
	clearSpriteData(1);
	setPlayerCursorColorCycling(1);
	setSpriteWidth(1, 1);
	setCursorSprite(sprite, height);
}

void initMenu(void) {
	menuIsHorizontal = 0;
	menuSelectedIndex = 0;
	menuEscapeCursorEvent = CursorNone;

	menuClickCallback = NULL;
	menuSelectedIndexCallback = NULL;
	menuEscapeCallback = NULL;
	registerCursorEventHandler(menuCursorHandler);
}
