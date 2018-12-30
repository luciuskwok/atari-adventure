// battle.c

#include "battle.h"
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "menu.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"

// Globals
UInt8 isLeavingBattle;
UInt8 menuType;
UInt8 rootMenuSelectedIndex;
UInt8 previousSelectedIndex;
UInt8 shouldRedrawEncounterTextOnMove;
GameChara enemy;


enum BattleMenuType {
	BattleRootMenu, BattleFightMenu, BattleItemMenu
};


static void applySelectionColor(UInt8 isMasking, UInt8 offset, UInt8 length) {
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

static void showEncounterText(void) {
	UInt8 s[64] = "* ";

	stringConcat(s, enemy.name);
	stringConcat(s, " blocks your path!");

	clearTextWindow(3);
	drawTextBox(s, 1, 0, 38, 2, -2);
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
	previousSelectedIndex = -1;
	setMenuSelectedIndex(rootMenuSelectedIndex);
}

static void drawEnemyHpBar(void) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 maxHp = charaMaxHp(&enemy);
	UInt8 width = maxHp / 2;
	UInt8 barX = 80 - width / 2;
	UInt8 fill = (enemy.hp + 1) / 2;

	drawBarChart(screen, barX, 48, width, fill);
}

static void enemyWasHit(UInt8 damage) {
	UInt8 i;

	// Show animation for when enemy was hit.
	for (i=0; i<4; ++i) {
		POKE(COLOR0, 0x32);
		waitVsync(4);
		POKE(COLOR0, 0x0E);
		waitVsync(4);
	}

	// Decrement enemy HP
	if (damage < enemy.hp) {
		enemy.hp -= damage;
	} else {
		enemy.hp = 0;
	}
	drawEnemyHpBar();

	if (enemy.hp == 0) {
		clearTextWindow(3);
		printString("* You are victorious.", 1, 0);
		waitForAnyInput();
		isLeavingBattle = 1;
	}
}

static void addPlayerExperience(GameCharaPtr /* chara */, UInt8 /* value */) {
	// TODO
}

static void charaAtIndexWasHit(UInt8 index, UInt8 damage) {
	GameCharaPtr chara = charaAtIndex(index);
	UInt8 i;

	// Show animation for player getting hit.
	for (i=0; i<4; ++i) {
		setSpriteHorizontalPosition(3, 48 + index * 40);
		waitVsync(4);
		setSpriteHorizontalPosition(3, 0);
		waitVsync(4);
	}

	// Decrement player HP
	if (damage < chara->hp) {
		chara->hp -= damage;
	} else {
		chara->hp = 0;
	}

	// Redraw the character's stats
	printCharaAtIndex(index, 4, 1);
}

static void doAttack(UInt8 player) {
	GameCharaPtr chara = charaAtIndex(player);
	UInt8 damage = 3;
	UInt8 s[40] = "* ";

	hideCursor();
	stringConcat(s, chara->name);
	clearTextWindow(3);

	if (chara->hp != 0) {
		stringConcat(s, " attacks.");
		printString(s, 1, 0);
	
		// Calculate damage to enemy or miss.
		enemyWasHit(damage);

		// Add to player XP
		addPlayerExperience(chara, 1);

		if (isLeavingBattle == 0) {
			// Pause before counter-attack
			waitVsync(15);
			stringCopy(s, "* ");
			stringConcat(s, enemy.name);
			stringConcat(s, " counter-attacks ");
			stringConcat(s, chara->name);
			stringConcat(s, ".");
			clearTextWindow(3);
			drawTextBox(s, 1, 0, 38, 2, -2);
	
			// Calculate to player character or miss.
			charaAtIndexWasHit(player, damage);
		}
	
		if (isLeavingBattle == 0) {
			enterRootMenu(0);
			shouldRedrawEncounterTextOnMove = 1;
		}
	} else {
		stringConcat(s, " seems unresponsive.");
		printString(s, 1, 0);
	}
}

static void enterFightMenu(void) {
	if (numberInParty() == 1) {
		doAttack(0);
	} else {

		clearTextWindow(3);
		printString("* Who shall fight?", 1, 0);

		menuType = BattleFightMenu;
		menuItemCount = 4;
		menuOrigin.x = 8;
		menuOrigin.y = 66;
		menuItemSpacing = 40;
		setMenuSelectedIndex(0);

		shouldRedrawEncounterTextOnMove = 0;
	}
}

static void enterTalk(void) {
	UInt8 s[64] = "* ";

	stringConcat(s, enemy.name);
	stringConcat(s, " doesn't care what you think!");

	clearTextWindow(3);
	drawTextBox(s, 1, 0, 38, 2, -2);
	shouldRedrawEncounterTextOnMove = 1;
}

static void useItem(UInt8 item) {
	UInt8 count = numberInParty();
	UInt8 i;
	UInt8 damage;
	GameCharaPtr chara;

	hideCursor();

	clearTextWindow(3);
	if (item == 0) {
		printString("* The Sacred Nuts cause a reaction.", 1, 0);
		damage = 8;
	} else {
		printString("* The Staff makes the earth move.", 1, 0);
		damage = 16;
	}
	
	enemyWasHit(damage);

	for (i=0; i<count; ++i) {
		chara = charaAtIndex(i);
		addPlayerExperience(chara, 1);
	}

	if (isLeavingBattle == 0) {
		enterRootMenu(0);
		shouldRedrawEncounterTextOnMove = 1;
	}
}

static void enterItemMenu(void) {
	const UInt8 x = 12;
	const UInt8 y = 1;
	const UInt8 spacing = 8;

	clearTextWindow(3);
	printString("* Nuts", x, y);
	printString("* Staff", x + spacing, y);

	menuType = BattleItemMenu;
	menuItemCount = 2;
	menuOrigin.x = 6 + 4 * x;
	menuOrigin.y = 53 + 4 * y;
	menuItemSpacing = 4 * spacing;
	setMenuSelectedIndex(0);

	shouldRedrawEncounterTextOnMove = 0;
}

static void attemptMercy(void) {
	isLeavingBattle = 1;
}

static SInt8 handleMenuClick(UInt8 index) {
	switch (menuType) {
		case BattleRootMenu:
			rootMenuSelectedIndex = menuSelectedIndex;
			switch (index) {
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
			doAttack(index);
			break;
		case BattleItemMenu:
			useItem(index);
			break;
	}
	return isLeavingBattle ? MessageReturnToMap : MessageNone;
}

static void handleMenuEscape(void) {
	if (menuType != BattleRootMenu) {
		enterRootMenu(1);
	}
}

static void menuSelectionDidChange(UInt8 index) {
	// Change color of old selection and new selection.
	if (menuType == BattleRootMenu) {
		// Remove old selection
		if (previousSelectedIndex < 4) {
			applySelectionColor(0, previousSelectedIndex * 10, 10);
		}

		// Apply new selection
		applySelectionColor(1, index * 10, 10);
		previousSelectedIndex = index;

		if (shouldRedrawEncounterTextOnMove) {
			showEncounterText();
		}
	}
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
	drawEnemyHpBar();

	// Draw button bar image
	err = drawImage(battleButtonsImage, battleButtonsImageLength, 49, 10);
	if (err) {
		debugPrint("puff() error:", err, 1, 1);
	}

	// Enemy counter-attack effect
	clearSpriteData(3);
	setSpriteWidth(3, 4);
	drawSprite(enemyAttackSprite, enemyAttackSpriteHeight, 3, 82);
	//setSpriteHorizontalPosition(3, 48);

	// Set up menu
	initMenu();
	menuIsHorizontal = 1;
	setMenuCursor(mediumHeartSprite, mediumHeartSpriteHeight);

	registerMenuDidClickCallback(handleMenuClick);
	registerMenuSelectedIndexDidChangeCallback(menuSelectionDidChange);
	menuEscapeCursorEvent = CursorDown;
	registerMenuDidEscapeCallback(handleMenuEscape);

	rootMenuSelectedIndex = 0;
	isLeavingBattle = 0;
	enterRootMenu(1);
}

