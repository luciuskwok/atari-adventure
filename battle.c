// battle.c

#include "battle.h"
#include "cursor.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"


// Globals
UInt8 isLeavingBattle;
UInt8 selectedIndex;
UInt8 menuItemCount;


void initBattle(void) {
	isLeavingBattle = 0;
	selectedIndex = 0;
	menuItemCount = 0;

	// Set up graphics window
	fadeOutColorTable(FadeTextBox);
	setScreenMode(ScreenModeOff);
	clearTextWindow();
	clearRasterScreen();
	setPlayerCursorVisible(0);

	hideSprites();

	// Turn on screen
	loadColorTable(temShopColorTable); // battleColorTable
	setTextBoxColors();
	setScreenMode(ScreenModeBattle);

	printStatText();

	// Draw background image
	{
		SInt8 err = drawImage(battleButtonsImage, battleButtonsImageLength, 62, 10);
		if (err) {
			UInt8 message[20] = "puff() error:";
			numberString(message+13, 0, err);
			printString(message, 1, 0);
		}
	}

	// Selection Cursor
	clearSpriteData(1);
	setPlayerCursorColorCycling(1);

	registerCursorEventHandler(battleCursorHandler);
}

void handleClick(void) {
	isLeavingBattle = 1;
}

void setSelectedIndex(UInt8 index) {
	selectedIndex = index;
}

SInt8 battleCursorHandler(UInt8 event) {
	if (event == CursorClick) {
		handleClick();
		if (isLeavingBattle != 0) {
			return MessageReturnToMap;
		} 
	} else {
		UInt8 newIndex = selectedIndex;
		switch (event) {
			case CursorUp:
				--newIndex;
				break;
			case CursorDown:
				++newIndex;
				break;
			case CursorLeft:
				break;
			case CursorRight:
				break;
		}
		if (newIndex != selectedIndex && newIndex < menuItemCount) {
			setSelectedIndex(newIndex);
		}
	}
	return MessageNone;
}
