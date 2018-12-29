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
UInt8 menuType;
UInt8 shouldRedrawEncounterTextOnMove;



void modifyPixelValues(UInt8 isMasking, UInt8 offset, UInt8 length) {
	const UInt8 y = 48;
	UInt8 height = 10;
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC) + y * 40;
	UInt8 x;
	UInt8 end = offset + length;

	while (height > 0) {
		for (x=0; x<40; ++x) {
			if (x >= offset && x < end) {
				if (isMasking) {
					*screen &= 0xAA;
				} else {
					*screen |= *screen >> 1;
				}
			}
			++screen;
		}
		--height;
	}
}


static void setSelectedIndex(UInt8 index) {
	PointU8 pt;
	pt.x = 43 * index + 9;
	pt.y = 91;
	setCursorPosition(&pt);

	// Change color of old selection and new selection.
	if (selectedIndex < 4) {
		modifyPixelValues(0, selectedIndex * 10, 10);
	}
	modifyPixelValues(1, index * 10, 10);

	selectedIndex = index;
}

static void showEncounterText(void) {
	PointU8 pt = { 6, 0 };
	UInt8 s[] = "* Evil Merchant blocks your path!";

	clearTextWindow(3);
	drawTextBox(s, &pt, 30, 2, -2);
	shouldRedrawEncounterTextOnMove = 0;
}

static void enterTalk(void) {
	PointU8 pt = { 6, 0 };
	UInt8 s[] = "* Evil Merchant doesn't care what you think!";

	clearTextWindow(3);
	drawTextBox(s, &pt, 30, 2, -2);
	shouldRedrawEncounterTextOnMove = 1;
}

static void enterItemMenu(void) {
	clearTextWindow(3);
}

static void attemptMercy(void) {
	isLeavingBattle = 1;
}

static void handleClick(void) {
	switch (selectedIndex) {
		case 0:
			break;
		case 1:
			enterTalk();
			break;
		case 2:
			enterItemMenu();
			break;
		case 3:
			attemptMercy();
			break;
	}
}

static SInt8 battleCursorHandler(UInt8 event) {
	if (event == CursorClick) {
		handleClick();
		if (isLeavingBattle != 0) {
			return MessageReturnToMap;
		} 
	} else {
		UInt8 newIndex = selectedIndex;
		switch (event) {
			case CursorUp:				
				break;
			case CursorDown:
				break;
			case CursorLeft:
				--newIndex;
				break;
			case CursorRight:
				++newIndex;
				break;
		}
		if (newIndex != selectedIndex) {
			if (shouldRedrawEncounterTextOnMove) {
				showEncounterText();
			}
			if (newIndex < menuItemCount) {
				setSelectedIndex(newIndex);
			}
		} 
	}
	return MessageNone;
}

void initBattle(void) {
	SInt8 err;

	// Set up graphics window
	fadeOutColorTable(FadeTextBox);
	setScreenMode(ScreenModeOff);
	clearTextWindow(7);
	clearRasterScreen();
	setPlayerCursorVisible(0);

	hideSprites();

	// Turn on screen
	loadColorTable(battleColorTable); // battleColorTable
	setScreenMode(ScreenModeBattle);

	printAllCharaText(4);

	// { // Debugging
	// 	UInt8 i;
	// 	for (i=0; i<128; ++i) {
	// 		textWindow[i+40] = i;
	// 	}
	// }

	showEncounterText();
	shouldRedrawEncounterTextOnMove = 0;

	// Draw button bar image
	err = drawImage(battleButtonsImage, battleButtonsImageLength, 48, 10);
	if (err) {
		UInt8 message[20] = "puff() error:";
		numberString(message+13, 0, err);
		printString(message, 1, 1);
	}

	// Draw enemy image
	err = drawImage(battleEnemyImage, battleEnemyImageLength, 0, 48);
	if (err) {
		UInt8 message[20] = "puff() error:";
		numberString(message+13, 0, err);
		printString(message, 1, 2);
	}
	
	// Selection Cursor
	isLeavingBattle = 0;
	menuItemCount = 4;
	clearSpriteData(1);
	setCursorSprite(mediumHeartSprite, mediumHeartSpriteHeight);
	selectedIndex = -1;
	setSelectedIndex(0);
	setPlayerCursorColorCycling(1);

	registerCursorEventHandler(battleCursorHandler);
}

