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
UInt8 menuType;
UInt8 menuItemCount;
PointU8 menuOrigin;
UInt8 menuItemSpacing;
UInt8 selectedIndex;
UInt8 rootMenuSelectedIndex;
UInt8 shouldRedrawEncounterTextOnMove;

enum BattleMenuType {
	BattleRootMenu, BattleFightMenu, BattleItemMenu
};


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
	pt.x = menuOrigin.x + menuItemSpacing * index;
	pt.y = menuOrigin.y;
	setCursorPosition(&pt);

	// Change color of old selection and new selection.
	if (menuType == BattleRootMenu) {
		if (selectedIndex < 4) {
			modifyPixelValues(0, selectedIndex * 10, 10);
		}
		modifyPixelValues(1, index * 10, 10);
	}

	selectedIndex = index;
}

static void showEncounterText(void) {
	PointU8 pt = { 3, 0 };
	UInt8 s[] = "* Evil Merchant blocks your path!";

	clearTextWindow(3);
	drawTextBox(s, &pt, 38, 2, -2);
	shouldRedrawEncounterTextOnMove = 0;
}

static void enterFightMenu(void) {
	rootMenuSelectedIndex = selectedIndex;

	clearTextWindow(3);
	printString("* Who shall fight?", 1, 0);

	menuType = BattleFightMenu;
	menuItemCount = 4;
	menuOrigin.x = 8;
	menuOrigin.y = 64;
	menuItemSpacing = 40;
	selectedIndex = -1;
	setSelectedIndex(0);

	shouldRedrawEncounterTextOnMove = 0;
}

static void enterTalk(void) {
	PointU8 pt = { 3, 0 };
	UInt8 s[] = "* Evil Merchant doesn't care what you think!";

	clearTextWindow(3);
	drawTextBox(s, &pt, 38, 2, -2);
	shouldRedrawEncounterTextOnMove = 1;
}

static void enterItemMenu(void) {
	rootMenuSelectedIndex = selectedIndex;

	clearTextWindow(3);
	printString("* Sacred Nuts (5)", 4, 0);
	printString("* Staff (1)", 24, 0);

	menuType = BattleItemMenu;
	menuItemCount = 2;
	menuOrigin.x = 17;
	menuOrigin.y = 49;
	menuItemSpacing = 80;
	selectedIndex = -1;
	setSelectedIndex(0);

	shouldRedrawEncounterTextOnMove = 0;
}

static void attemptMercy(void) {
	isLeavingBattle = 1;
}

static void enterRootMenu(void) {
	showEncounterText();

	menuType = BattleRootMenu;
	menuItemCount = 4;
	menuOrigin.x = 9;
	menuOrigin.y = 90;
	menuItemSpacing = 43;
	selectedIndex = -1;
	setSelectedIndex(rootMenuSelectedIndex);
}

static void handleClick(void) {
	switch (menuType) {
		case BattleRootMenu:
			switch (selectedIndex) {
				case 0:
					enterFightMenu();
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
			break;
		case BattleFightMenu:
			break;
		case BattleItemMenu:
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
			case CursorLeft:
				--newIndex;
				break;
			case CursorRight:
				++newIndex;
				break;
			case CursorDown:
				if (menuType != BattleRootMenu) {
					enterRootMenu();
					return MessageNone;
				}
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
		debugPrint("puff() error:", err, 1, 1);
	}

	// Draw enemy image
	err = drawImage(battleEnemyImage, battleEnemyImageLength, 0, 48);
	if (err) {
		debugPrint("puff() error:", err, 1, 2);
	}

	// Selection Cursor
	clearSpriteData(1);
	setCursorSprite(mediumHeartSprite, mediumHeartSpriteHeight);
	setPlayerCursorColorCycling(1);

	// Set up menu
	rootMenuSelectedIndex = 0;
	isLeavingBattle = 0;
	enterRootMenu();

	registerCursorEventHandler(battleCursorHandler);
}

