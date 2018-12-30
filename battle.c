// battle.c

#include "battle.h"
#include "cursor.h"
#include "game_chara.h"
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
GameChara enemy;


enum BattleMenuType {
	BattleRootMenu, BattleFightMenu, BattleItemMenu
};


void modifyPixelValues(UInt8 isMasking, UInt8 offset, UInt8 length) {
	const UInt8 y = 49;
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
	UInt8 s[64] = "* ";

	stringConcat(s, enemy.name);
	stringConcat(s, " blocks your path!");

	clearTextWindow(3);
	drawTextBox(s, &pt, 38, 2, -2);
	shouldRedrawEncounterTextOnMove = 0;
}

static void enterRootMenu(UInt8 showText) {
	if (showText) {
		showEncounterText();
	}

	menuType = BattleRootMenu;
	menuItemCount = 4;
	menuOrigin.x = 9;
	menuOrigin.y = 92;
	menuItemSpacing = 43;
	selectedIndex = -1;
	setSelectedIndex(rootMenuSelectedIndex);
}

static void waitVsync(UInt8 ticks) {
	*VB_TIMER = ticks;
	while (*VB_TIMER != 0) {} // Wait for VSYNC 
}

static void enemyWasHit(void) {
	UInt8 i;

	// Show animation for when enemy was hit.
	for (i=0; i<4; ++i) {
		POKE(COLOR0, 0x32);
		waitVsync(4);
		POKE(COLOR0, 0x0E);
		waitVsync(4);
	}	
}

static void doAttack(UInt8 /* player */) {
	clearTextWindow(3);
	printString("* You attack.", 1, 0);

	enemyWasHit();

	shouldRedrawEncounterTextOnMove = 1;
}

static void enterFightMenu(void) {
	if (numberInParty() == 1) {
		doAttack(0);
	} else {
		rootMenuSelectedIndex = selectedIndex;

		clearTextWindow(3);
		printString("* Who shall fight?", 1, 0);

		menuType = BattleFightMenu;
		menuItemCount = 4;
		menuOrigin.x = 8;
		menuOrigin.y = 66;
		menuItemSpacing = 40;
		selectedIndex = -1;
		setSelectedIndex(0);

		shouldRedrawEncounterTextOnMove = 0;
	}
}

static void enterTalk(void) {
	const PointU8 pt = { 3, 0 };
	UInt8 s[64] = "* ";

	stringConcat(s, enemy.name);
	stringConcat(s, " doesn't care what you think!");

	clearTextWindow(3);
	drawTextBox(s, &pt, 38, 2, -2);
	shouldRedrawEncounterTextOnMove = 1;
}

static void useItem(UInt8 item) {
	const PointU8 pt = { 3, 0 };
	const UInt8 width = 38;
	const UInt8 lineSpacing = 1;
	const SInt8 indent = -2;

	hideCursor();

	clearTextWindow(3);
	if (item == 0) {
		//printString("* You throw the Sacred Nuts.", 1, 0);
		drawTextBox("* You throw the Sacred Nuts.", &pt, width, lineSpacing, indent);
	} else {
		drawTextBox("* You feel the earth move.", &pt, width, lineSpacing, indent);
	}
	
	enemyWasHit();
	//waitForAnyInput();

	enterRootMenu(0);
	shouldRedrawEncounterTextOnMove = 1;
}

static void enterItemMenu(void) {
	const UInt8 x = 12;
	const UInt8 y = 1;
	const UInt8 spacing = 8;

	rootMenuSelectedIndex = selectedIndex;

	clearTextWindow(3);
	printString("* Nuts", x, y);
	printString("* Staff", x + spacing, y);

	menuType = BattleItemMenu;
	menuItemCount = 2;
	menuOrigin.x = 6 + 4 * x;
	menuOrigin.y = 53 + 4 * y;
	menuItemSpacing = 4 * spacing;
	selectedIndex = -1;
	setSelectedIndex(0);

	shouldRedrawEncounterTextOnMove = 0;
}

static void attemptMercy(void) {
	isLeavingBattle = 1;
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
			doAttack(selectedIndex);
			break;
		case BattleItemMenu:
			useItem(selectedIndex);
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
					enterRootMenu(1);
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

	// Set up enemy character
	stringCopy(enemy.name, "Evil Merchant");
	enemy.level = 12;
	enemy.hp = charaMaxHp(&enemy);
	showEncounterText();
	shouldRedrawEncounterTextOnMove = 0;

	// Draw enemy image
	err = drawImage(battleEnemyImage, battleEnemyImageLength, 0, 48);
	if (err) {
		debugPrint("puff() error:", err, 1, 2);
	}

	// Draw enemy HP bar
	{
		UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
		UInt8 maxHp = charaMaxHp(&enemy);
		UInt8 width = maxHp / 2;
		UInt8 barX = 80 - width / 2;
		UInt8 fill = (enemy.hp + 1) / 2;

		drawBarChart(screen, barX, 48, width, fill);
	}

	// Draw button bar image
	err = drawImage(battleButtonsImage, battleButtonsImageLength, 49, 10);
	if (err) {
		debugPrint("puff() error:", err, 1, 1);
	}

	// Selection Cursor
	clearSpriteData(1);
	setCursorSprite(mediumHeartSprite, mediumHeartSpriteHeight);
	setPlayerCursorColorCycling(1);

	// Set up menu
	rootMenuSelectedIndex = 0;
	isLeavingBattle = 0;
	enterRootMenu(1);

	registerCursorEventHandler(battleCursorHandler);
}

