// battle.c

#include "battle.h"
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "menu.h"
#include "sound.h"
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

#define enemyHpBarRow (44)

enum BattleMenuType {
	BattleRootMenu, BattleFightMenu, BattleItemMenu
};

#define enemyAttackSpritePlayer (4)

const UInt8 battleLoadingColorTable[] = {
	0x00, 0x00, 0x00, 0x00, // unused, player sprites hidden
	0x08, 0x04, 0x00, // playfield
	0x06, // missile sprites
	0x00, // background
	0x00, 0x00, 0x00 // unused, DLI disabled
};

const UInt8 battleColorTable[] = {
	0x58, // cursor sprite
	0x00, // unused sprite 
	0x0A, // chara level text
	0x32, // enemy attack sprite
	0x0E, 0x1E, 0x28, // playfield
	0x0A, // missile sprites
	0x00, // background
	0x0E, // chara name text
	0x30, // dialog box background
	0x82  // hp bar background
};

const DataBlock battleButtonsImage = {
	164, // length
	{
		0x8D,0x90,0x31,0x0A,0x05,0x31,0x08,0x44,0x05,0x5B,0x21,0x57,0x11,0x6C,0x17,0xBC,
		0xBA,0x60,0x1B,0xD8,0xAB,0x7C,0x48,0x1B,0xF0,0xAB,0x59,0xD8,0x76,0xA7,0x1A,0xE1,
		0x65,0x26,0x1A,0xD1,0xDA,0x00,0x78,0x5C,0xFC,0x00,0xC6,0x63,0x0D,0x40,0x5F,0x5B,
		0xA2,0xE4,0x8E,0x03,0xEE,0xA9,0x65,0x3D,0x95,0xD0,0x50,0xA7,0xF2,0x3D,0x9B,0x0B,
		0x47,0xA4,0xE2,0xD4,0xEE,0x1D,0xCC,0xCD,0xB1,0xDC,0xBE,0xC4,0x88,0xC9,0xD8,0x2B,
		0x6F,0x00,0x91,0x23,0x17,0x37,0xC0,0xCC,0xAE,0x5F,0x71,0xFB,0x5A,0x24,0x28,0xC6,
		0x4A,0xA6,0xCD,0x41,0xBE,0x11,0xB4,0x93,0x67,0xB6,0xA5,0xF2,0x24,0x44,0x66,0x72,
		0xB7,0x31,0xF9,0xC9,0xBB,0x98,0x74,0x1D,0x6E,0x27,0x67,0xDD,0x2B,0xC8,0x32,0x1C,
		0x96,0xBF,0x1C,0x72,0xE5,0x75,0xEF,0x7A,0xB9,0x60,0x21,0x07,0x99,0xC9,0xE9,0xE1,
		0x04,0x92,0x5B,0x67,0x8F,0xFC,0xDF,0xB3,0xC7,0xE6,0xEC,0xCD,0xB8,0x8F,0x77,0x89,
		0x8F,0x77,0xFE,0x03
	}
};

const DataBlock battleEnemyImage = {
	163, // length
	{
		0xED,0xD4,0x31,0x0E,0x83,0x30,0x0C,0x05,0xD0,0x40,0xC5,0xC2,0xC2,0x94,0xF3,0x34,
		0x47,0x60,0xE0,0xDF,0x87,0xA3,0x30,0x22,0x9F,0x02,0x89,0xAD,0xA7,0xA9,0x3A,0x76,
		0xA8,0x1B,0x90,0x80,0x50,0x6A,0xE1,0x21,0xAA,0x32,0xD4,0xF3,0x53,0x64,0x7D,0xDB,
		0x31,0xE6,0x5F,0x91,0xAB,0x68,0x74,0x0C,0x50,0x41,0x0B,0xA0,0x3E,0x67,0x99,0x67,
		0x9A,0x07,0x8B,0xC9,0x21,0x9E,0xB3,0xB3,0x73,0xC9,0x3A,0x24,0xEF,0x32,0xB8,0xC4,
		0xFA,0xAB,0x98,0x87,0xC0,0x39,0xC9,0x31,0xF3,0x5D,0x33,0x8F,0xA3,0xFB,0xCA,0x2E,
		0x93,0xA3,0x36,0xD8,0xBF,0x46,0x74,0x0F,0xEA,0x96,0x06,0x6B,0x23,0xB8,0xAB,0x77,
		0xB4,0x3A,0x2B,0xAE,0xF3,0xDE,0x19,0x31,0x3D,0xEF,0x5E,0x44,0xFD,0x69,0xB4,0xDE,
		0x3D,0x37,0x27,0xDF,0xF9,0xC1,0xE1,0x47,0x6E,0xE4,0x1B,0x2D,0x39,0xCF,0x4E,0x5C,
		0x97,0x92,0x28,0x1C,0x9C,0xE8,0x72,0xA5,0xFB,0x38,0xE0,0x98,0x0E,0xBA,0x7F,0x32,
		0x31,0xF7,0x06
	}
};

const DataBlock enemyAttackSprite = {
	14, // length
	{ 0x18, 0x3C, 0x7E, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x7E, 0x3C, 0x18 }
};

const UInt8 battleGradient[] = {
	0xF2, 0x80, 0xFB, 0x72, 0xFE, 0x64, 0x01, 0x56, 0x48, 0xFC, 0x90, 0x01, 
	0xA2, 0xA2, 0xFE, 0xB4, 0xFD, 0xC6, 0xFC, 0xD8, 0xFE, 0xEA,
	0x80 // terminator
};


static void exitBattle(void) {
	isLeavingBattle = 1;
}

static void applySelectionColor(UInt8 isMasking, UInt8 offset, UInt8 length) {
	const UInt8 y = enemyHpBarRow + 1;
	UInt8 height = 10;
	UInt8 *screen = GRAPHICS_WINDOW + y * 40;
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

static void clearDialogBox(void) {
	zeroOut8(TEXT_WINDOW, 5*SCREEN_ROW_BYTES);
}

static void printInDialogBox(const UInt8 *s) {
	// COLCRS: starting & newline X-position.
	POKE(COLCRS, 1);

	// ROWCRS: starting Y-position.
	POKE(ROWCRS, 1);

	// LMARGN: X-position after wrapping line
	POKE(LMARGN, 3);

	// RMARGN: X-position of right margin
	POKE(RMARGN, 39);

	// ROWINC: line spacing
	POKE(ROWINC, 2);

	// Print text within default text box
	drawTextBox(s);
}

static void showEncounterText(void) {
	UInt8 s[64] = "* ";

	stringConcat(s, enemy.name);
	stringConcat(s, " blocks your path!");

	clearDialogBox();
	printInDialogBox(s);
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
	UInt8 maxHp = charaMaxHp(&enemy);
	UInt8 width = maxHp / 2;
	UInt8 barX = 80 - width / 2;
	UInt8 fill = (enemy.hp + 1) / 2;

	POKEW(SAVADR, (UInt16)GRAPHICS_WINDOW + enemyHpBarRow * SCREEN_ROW_BYTES);
	POKE(COLCRS, barX);
	POKE(DELTAC, width);
	POKE(COLINC, fill);

	drawBarChart();
}

static void enemyWasHit(UInt8 damage) {
	UInt8 i;

	// Show animation for when enemy was hit.
	for (i=0; i<4; ++i) {
		POKE(COLOR0, 0x32);
		delayTicks(4);
		POKE(COLOR0, 0x0E);
		delayTicks(4);
	}

	// Decrement enemy HP
	if (damage < enemy.hp) {
		enemy.hp -= damage;
	} else {
		enemy.hp = 0;
	}
	drawEnemyHpBar();

	if (enemy.hp == 0) {
		clearDialogBox();
		printInDialogBox("* You are victorious.");
		waitForAnyInput();
		exitBattle();
	}
}

static void addPlayerExperience(GameCharaPtr /* chara */, UInt8 /* value */) {
	// TODO
}

static void charaAtIndexWasHit(UInt8 index, UInt8 damage) {
	GameCharaPtr chara = charaAtIndex(index);
	UInt8 i;

	// Play sound
	noteOn(NoteC+Oct5, 2, 15, 1, 0x80, 3);

	// Show animation for player getting hit.
	for (i=0; i<4; ++i) {
		setSpriteOriginX(enemyAttackSpritePlayer, 48 + index * 40);
		delayTicks(4);
		setSpriteOriginX(enemyAttackSpritePlayer, 0);
		delayTicks(4);
	}

	// Decrement player HP
	if (damage < chara->hp) {
		chara->hp -= damage;
	} else {
		chara->hp = 0;
	}

	// Redraw the character's stats
	POKE(ROWCRS, 5);
	eraseCharaBoxAtIndex(index);
	printCharaAtIndex(index);
}

static void doAttack(UInt8 player) {
	GameCharaPtr chara = charaAtIndex(player);
	UInt8 damage = 3;
	UInt8 s[40] = "* ";

	hideCursor();
	stringConcat(s, chara->name);
	clearDialogBox();

	if (chara->hp != 0) {
		stringConcat(s, " attacks.");
		printInDialogBox(s);

		// Play sound
		noteOn(NoteF+Oct5, 2, 15, 1, 0x80, 3);
	
		// Calculate damage to enemy or miss.
		enemyWasHit(damage);

		// Add to player XP
		addPlayerExperience(chara, 1);

		if (isLeavingBattle == 0) {
			// Pause before counter-attack
			delayTicks(15);
			stringCopy(s, "* ");
			stringConcat(s, enemy.name);
			stringConcat(s, " counter-attacks ");
			stringConcat(s, chara->name);
			stringConcat(s, ".");
			clearDialogBox();
			printInDialogBox(s);
	
			// Calculate to player character or miss.
			charaAtIndexWasHit(player, damage);
		}
	
		if (isLeavingBattle == 0) {
			enterRootMenu(0);
			shouldRedrawEncounterTextOnMove = 1;
		}
	} else {
		stringConcat(s, " seems unresponsive.");
		printInDialogBox(s);
	}
}

static void enterFightMenu(void) {
	if (partySize == 1) {
		doAttack(0);
	} else {
		clearDialogBox();
		printInDialogBox("* Who shall fight?");

		menuType = BattleFightMenu;
		menuItemCount = partySize;
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

	clearDialogBox();
	printInDialogBox(s);
	shouldRedrawEncounterTextOnMove = 1;
}

static void useItem(UInt8 item) {
	UInt8 i;
	UInt8 damage;
	GameCharaPtr chara;

	hideCursor();

	clearDialogBox();
	if (item == 0) {
		// Play sound
		noteOn(NoteC+Oct5, 8, 15, 1, 0x80, 3);
		printInDialogBox("* The Sacred Nuts cause a reaction.");
		damage = 8;
	} else {
		// Play sound
		noteOn(NoteF+Oct4, 16, 15, 1, 0x80, 3);

		printInDialogBox("* The Staff makes the earth move.");
		damage = 16;
	}
	
	enemyWasHit(damage);

	for (i=0; i<partySize; ++i) {
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
	const UInt8 y = 2;
	const UInt8 spacing = 8;

	clearDialogBox();
	printStringAtXY("* Nuts", x, y);
	printStringAtXY("* Staff", x + spacing, y);

	menuType = BattleItemMenu;
	menuItemCount = 2;
	menuOrigin.x = 6 + 4 * x;
	menuOrigin.y = 47 + 4 * y;
	menuItemSpacing = 4 * spacing;
	setMenuSelectedIndex(0);

	shouldRedrawEncounterTextOnMove = 0;
}

static void attemptMercy(void) {
	exitBattle();
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
	UInt16 startTime = SHORT_CLOCK;
	// Use block of memory at end of graphics screen memory area for gradient.
	UInt8 *unpackedGradient = GRAPHICS_WINDOW + (enemyHpBarRow+11) * SCREEN_ROW_BYTES;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	zeroOut16(TEXT_WINDOW, 8*SCREEN_ROW_BYTES);
	zeroOut16(GRAPHICS_WINDOW, (enemyHpBarRow+11)*SCREEN_ROW_BYTES);
	setPlayerCursorVisible(0);

	// Set loading color table and disable DLI
	loadColorTable(battleLoadingColorTable);
	setScreenMode(ScreenModeBattle);
	POKE(NMIEN, 0x40);

	// Start battle music
	startSong(8);

	printAllCharaStats(5);

	// { // Debugging
	// 	UInt8 i;
	// 	for (i=0; i<128; ++i) {
	// 		TEXT_WINDOW[i+40] = i;
	// 	}
	// }

	// Set up enemy character
	stringCopy(enemy.name, "Evil Merchant");
	enemy.level = 12;
	enemy.hp = charaMaxHp(&enemy);
	showEncounterText();
	shouldRedrawEncounterTextOnMove = 0;

	// Set the background color gradient
	unpackbits(unpackedGradient, battleGradient);
	setDliColorTable(unpackedGradient);

	// Set up missiles
	clearSprite(0);  
	fillSprite(0, 0xFF, 0, 10); // Sound sprites for debugging
	fillSprite(0, 0xFF, 10+48-18, 18); // Perspective lines

	// Draw enemy image
	drawImage(&battleEnemyImage, 0);

	// Draw enemy HP bar
	drawEnemyHpBar();

	// Draw button bar image
	drawImage(&battleButtonsImage, enemyHpBarRow+1);

	// Enemy counter-attack effect
	clearSprite(enemyAttackSpritePlayer);
	setSpriteWidth(enemyAttackSpritePlayer, 4);
	drawSprite(&enemyAttackSprite, enemyAttackSpritePlayer, 82);

	// Set up menu
	initMenu();
	menuIsHorizontal = 1;
	setMenuCursor(MediumHeartCursor);

	setMenuClickHandler(handleMenuClick);
	setMenuSelIndexHandler(menuSelectionDidChange);
	menuEscapeCursorEvent = CursorDown;
	setMenuEscapeHandler(handleMenuEscape);

	rootMenuSelectedIndex = 0;
	isLeavingBattle = 0;
	enterRootMenu(1);

	// Switch to final color table and enable DLI
	loadColorTable(battleColorTable);
	POKE(NMIEN, 0xC0);

	// Copy first color to background color shadow register so that the color extends to overscan.
	POKE(COLOR4, unpackedGradient[0]);  

	debugPrint("Init:", SHORT_CLOCK - startTime, 30, 3);
}

