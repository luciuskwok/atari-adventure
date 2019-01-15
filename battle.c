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
#include <zlib.h>


// Globals
UInt8 isLeavingBattle;
UInt8 menuType;
UInt8 rootMenuSelectedIndex;
UInt8 previousSelectedIndex;
UInt8 shouldRedrawEncounterTextOnMove;
GameChara enemy;

#define graphicsHeight (44)

enum BattleMenuType {
	BattleRootMenu, BattleFightMenu, BattleItemMenu
};

#define enemyAttackSpritePlayer (4)

const UInt8 battleLoadingColorTable[] = {
	0x00, 0x00, 0x00, 0x00, // unused, player sprites hidden
	0x06, 0x0E, 0x02, // playfield
	0x06, // missile sprites
	0x00, // background
	0x00, 0x00, 0x00 // unused, DLI disabled
};

const UInt8 battleColorTable[] = {
	0x58, // cursor sprite
	0x00, // unused sprite 
	0x0A, // chara level text
	0x32, // enemy attack sprite
	0x3E, 0x0E, 0x00, // playfield
	0x28, // normal button color, missile sprites
	0x00, // background
	0x0E, // chara name text, hp bar foreground
	0x30, // dialog box background
	0x82  // hp bar background
};

const UInt8 steveFace[] = {
	0x00,0x00,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0x3F,0x00,0x00,0x00,
	0x00,0xFF,0xFF,0xFF,0xFF,0xC0,0x00,0x00,0x03,0xFF,0xFF,0xFF,0xFF,0xC0,0x00,0x00,
	0x03,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x0F,0xFF,0xFF,0xFD,0xFF,0xF0,0x00,0x00,
	0x0F,0xFF,0xFF,0xFD,0xFF,0xFC,0x00,0x00,0x3F,0xFF,0xFF,0xD5,0x7F,0xFC,0x00,0x00,
	0x3F,0xFF,0xFD,0x55,0x5F,0xFF,0x00,0x00,0x3F,0xFF,0xD5,0x55,0x5F,0xFF,0x00,0x00,
	0xFF,0xFF,0xF5,0x5F,0xFF,0xFF,0x00,0x00,0xFF,0xFF,0xDD,0x7D,0x57,0xFF,0x00,0x00,
	0xFF,0xD7,0x57,0x55,0xD7,0xFF,0x00,0x00,0xFF,0xD7,0x57,0x55,0xD5,0xFF,0x00,0x00,
	0x3F,0x55,0x57,0x55,0x55,0xFC,0x00,0x00,0x3F,0x55,0x57,0x55,0x55,0x7C,0x00,0x00,
	0x3F,0x55,0x57,0x55,0x55,0x70,0x00,0x00,0x0F,0xD5,0x57,0x5D,0x55,0xF0,0x00,0x00,
	0x0F,0xD5,0x55,0xF5,0x55,0xF0,0x00,0x00,0x03,0xF5,0x55,0x55,0x57,0xF0,0x00,0x00,
	0x03,0xF5,0x55,0x55,0x57,0xC0,0x00,0x00,0x00,0x3D,0x5D,0x57,0x57,0xC0,0x00,0x00,
	0x00,0x0D,0x57,0xFD,0x5F,0x00,0x00,0x00,0x00,0x3B,0x55,0x55,0x5C,0x00,0x00,0x00,
	0x00,0xEB,0xD5,0x55,0x7B,0x00,0x00,0x00,0x03,0xEB,0x7D,0x57,0xFA,0xC0,0x00,0x00,
	0x3F,0xEA,0xD7,0xFD,0x7A,0xFC,0x00,0x00,0xFF,0xEA,0xB5,0x55,0xEA,0xFF,0xC0,0x00,
	0xFF,0xEA,0xAF,0x5F,0xAA,0xFF,0xFC,0x00,0xFF,0xFA,0xAA,0xFA,0xAB,0xFF,0xFF,0xC0,
	0xFF,0xFA,0xAF,0xFE,0xAB,0xFF,0xFF,0xF0,0xFF,0xFE,0xBB,0xFE,0xAF,0xFF,0xFF,0xFC,
};

const DataBlock battleBackgroundImage = {
	326, { /// length
		0x78,0xDA,0xED,0x92,0xC1,0x8A,0xC4,0x20,0x10,0x44,0x1B,0xBC,0x0A,0xF9,0x15,0x21,
		0xD7,0x80,0xBF,0x2E,0xE4,0x2A,0xF8,0x2B,0x82,0x57,0xA1,0xA6,0xAA,0x4D,0xB2,0x33,
		0x97,0x3D,0xEE,0x5E,0xA6,0xC7,0x99,0x38,0x5A,0xBC,0x7E,0x31,0x31,0xFB,0xD6,0xB7,
		0xBE,0xF5,0x97,0x95,0x9F,0x59,0xFC,0x2D,0x16,0x30,0xEF,0x29,0xFA,0xE7,0xD6,0xF1,
		0x96,0xEA,0x19,0x28,0x84,0x32,0xB2,0xF9,0xCC,0x2C,0x3D,0x84,0x7E,0x76,0x5D,0x84,
		0xF0,0x32,0x86,0xBB,0xCF,0x7D,0xBD,0xF7,0x6E,0xDB,0x4C,0xD9,0x17,0x02,0xFE,0xB1,
		0xEC,0xED,0x4C,0xD6,0x27,0x98,0x15,0xDE,0xC9,0x35,0xB9,0xAB,0xD6,0xDA,0x8E,0x39,
		0x27,0xFA,0x86,0x80,0x13,0x03,0x13,0xC8,0x68,0xE8,0x98,0x1B,0x0C,0xBA,0xD1,0xC8,
		0xD0,0x9D,0x4F,0xD1,0x4A,0x89,0xC9,0xAA,0xCE,0x3A,0xD9,0x6E,0x21,0x5A,0x4A,0xC1,
		0xD4,0x51,0xD4,0x3D,0xD5,0xBD,0xE5,0x46,0x1A,0xCF,0xF1,0x14,0x6A,0x80,0x34,0x42,
		0x3B,0x0F,0x93,0x29,0x88,0x37,0xC7,0x43,0x8B,0x1C,0xB4,0x92,0x9A,0xDC,0x92,0x80,
		0x21,0x50,0x92,0x34,0x9A,0xC6,0xFD,0x80,0x68,0x94,0x13,0xCE,0x61,0xD7,0x2F,0x81,
		0x54,0x24,0xB1,0xD5,0x52,0x4A,0x6D,0x1E,0x63,0x8B,0x99,0x7D,0x7B,0xE5,0x38,0xC0,
		0x65,0x3D,0x54,0x9B,0x23,0xC6,0xD5,0x94,0xCA,0x66,0xA7,0x79,0x3B,0xF5,0x2D,0x1A,
		0x96,0xF8,0x2D,0x65,0xB5,0x6E,0x2D,0x6D,0x8B,0x2F,0x1C,0xF5,0xF1,0x73,0x11,0x50,
		0x3E,0x6C,0xBD,0x1F,0xFD,0xFE,0x43,0x3B,0x75,0x53,0x06,0xCB,0x40,0x86,0x8E,0x80,
		0x55,0x8F,0x15,0x2C,0x9C,0x36,0xEF,0x9C,0x04,0x1D,0x28,0x45,0xF8,0x1B,0x2C,0x39,
		0xD9,0xB9,0xDB,0xEE,0x4F,0xC9,0x1D,0x97,0xA1,0x14,0xCD,0xC6,0x31,0x9C,0xEC,0xB8,
		0x89,0x8B,0xD3,0x2E,0x41,0x07,0xD2,0xAA,0xD5,0xD8,0x5C,0xCE,0xED,0x3C,0xD3,0x3E,
		0x26,0x32,0xA4,0xE2,0x1C,0x67,0x2E,0xB8,0x71,0xBE,0xF5,0xE4,0x1C,0xBC,0x80,0x7C,
		0xC7,0x5F,0x07,0x20,0x02,0xB1,
	}
};

const DataBlock battleButtonsImage = {
	170, { /// length
		0x78,0xDA,0x8D,0x90,0x31,0x0A,0x05,0x31,0x08,0x44,0x05,0x5B,0x21,0x57,0x11,0x6C,
		0x17,0xBC,0xBA,0x60,0x1B,0xD8,0xAB,0x7C,0x48,0x1B,0xF0,0xAB,0x59,0xD8,0x76,0xA7,
		0x1A,0xE1,0x65,0x26,0x1A,0xD1,0xDA,0x00,0x78,0x5C,0xFC,0x00,0xC6,0x63,0x0D,0x40,
		0x5F,0x5B,0xA2,0xE4,0x8E,0x03,0xEE,0xA9,0x65,0x3D,0x95,0xD0,0x50,0xA7,0xF2,0x3D,
		0x9B,0x0B,0x47,0xA4,0xE2,0xD4,0xEE,0x1D,0xCC,0xCD,0xB1,0xDC,0xBE,0xC4,0x88,0xC9,
		0xD8,0x2B,0x6F,0x00,0x91,0x23,0x17,0x37,0xC0,0xCC,0xAE,0x5F,0x71,0xFB,0x5A,0x24,
		0x28,0xC6,0x4A,0xA6,0xCD,0x41,0xBE,0x11,0xB4,0x93,0x67,0xB6,0xA5,0xF2,0x24,0x44,
		0x66,0x72,0xB7,0x31,0xF9,0xC9,0xBB,0x98,0x74,0x1D,0x6E,0x27,0x67,0xDD,0x2B,0xC8,
		0x32,0x1C,0x96,0xBF,0x1C,0x72,0xE5,0x75,0xEF,0x7A,0xB9,0x60,0x21,0x07,0x99,0xC9,
		0xE9,0xE1,0x04,0x92,0x5B,0x67,0x8F,0xFC,0xDF,0xB3,0xC7,0xE6,0xEC,0xCD,0xB8,0x8F,
		0x77,0x89,0x8F,0x77,0xFE,0x03,0x99,0x37,0x8E,0xAB,
	}
};

const DataBlock enemyAttackSprite = {
	14, // length
	{ 0x18, 0x3C, 0x7E, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x7E, 0x3C, 0x18 }
};

const UInt8 battleGradient[] = {
	0xF2, 0x80, 0xFB, 0x72, 0xFE, 0x64, 0x01, 0x56, 0x48, 0xFE, 0x00, 0x03, 0x90, 
	0x90, 0xA2, 0xA2, 0xFE, 0xB4, 0xFD, 0xC6, 0xFC, 0xD8, 0xFE, 0xEA,
	0x80 // terminator
};


static void exitBattle(void) {
	isLeavingBattle = 1;
}

static void applySelectionColor(UInt8 isMasking, UInt8 offset, UInt8 length) {
	const UInt8 y = graphicsHeight + 1;
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
	POKE(COLCRS, 1);	// COLCRS: starting & newline X-position.
	POKE(ROWCRS, 1);	// ROWCRS: starting Y-position.
	POKE(LMARGN, 3);	// LMARGN: X-position after wrapping line
	POKE(RMARGN, 39);	// RMARGN: X-position of right margin
	POKE(ROWINC, 2);	// ROWINC: line spacing
	drawTextBox(s); 	// Print text within default text box
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
	UInt8 maxHp = maxHpWithCharaLevel(enemy.level);
	UInt8 width = maxHp / 2;
	UInt8 barX = 80 - width / 2;
	UInt8 fill = (enemy.hp + 1) / 2;

	POKEW(SAVADR, (UInt16)GRAPHICS_WINDOW + graphicsHeight * SCREEN_ROW_BYTES);
	POKE(COLCRS, barX);
	POKE(DELTAC, width);
	POKE(COLINC, fill);

	drawBarChart();
}

static void enemyWasHit(UInt8 damage) {
	UInt8 originalColor = PEEK(COLOR0);
	UInt8 i;

	// Show animation for when enemy was hit.
	for (i=0; i<4; ++i) {
		POKE(COLOR0, 0x32);
		delayTicks(4);
		POKE(COLOR0, 0x0E);
		delayTicks(4);
	}
	POKE(COLOR0, originalColor);

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

static void addPlayerExperience(UInt8 /* charaIndex */, UInt8 /* value */) {
	// TODO
}

static void charaAtIndexWasHit(UInt8 index, UInt8 damage) {
	UInt8 x = 1 + index * 10;
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
	if (damage < partyChara[index].hp) {
		partyChara[index].hp -= damage;
	} else {
		partyChara[index].hp = 0;
	}

	// Redraw the character's stats
	POKE(ROWCRS, 5);
	POKE(COLCRS, x);
	eraseCharaBoxAtIndex(index);
	printCharaAtIndex(index);
}

static void doAttack(UInt8 player) {
	UInt8 damage = 3;
	UInt8 s[40] = "* ";

	hideCursor();
	stringConcat(s, partyChara[player].name);
	clearDialogBox();

	if (partyChara[player].hp != 0) {
		stringConcat(s, " attacks.");
		printInDialogBox(s);

		// Play sound
		noteOn(NoteF+Oct5, 2, 15, 1, 0x80, 3);
	
		// Calculate damage to enemy or miss.
		enemyWasHit(damage);

		// Add to player XP
		addPlayerExperience(player, 1);

		if (isLeavingBattle == 0) {
			// Pause before counter-attack
			delayTicks(15);
			stringCopy(s, "* ");
			stringConcat(s, enemy.name);
			stringConcat(s, " counter-attacks ");
			stringConcat(s, partyChara[player].name);
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
		menuOrigin.x = 9;
		menuOrigin.y = 70;
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
		addPlayerExperience(i, 1);
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
	menuOrigin.y = 48 + 4 * y;
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

static void drawEnemyCharaImage(const UInt8 *image) {
	POKE(COLCRS, 20 - 4);
	POKE(ROWCRS, 44 - 32);
	POKE(DELTAR, 32);
	POKE(DELTAC, 8);

	compositeImage(image);
	return;
}

void initBattle(void) {
	UInt16 startTime = SHORT_CLOCK;
	// Use block of memory at end of graphics screen memory area for gradient.
	UInt8 *unpackedGradient = GRAPHICS_WINDOW + (graphicsHeight+11) * SCREEN_ROW_BYTES;
	int err; 

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	zeroOut16(TEXT_WINDOW, 8*SCREEN_ROW_BYTES);
	zeroOut16(GRAPHICS_WINDOW, (graphicsHeight+11)*SCREEN_ROW_BYTES);
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
	stringCopy(enemy.name, "Steve Jobs");
	enemy.level = 12;
	enemy.hp = maxHpWithCharaLevel(enemy.level);
	showEncounterText();
	shouldRedrawEncounterTextOnMove = 0;

	// Enemy counter-attack effect
	clearSprite(enemyAttackSpritePlayer);
	setSpriteWidth(enemyAttackSpritePlayer, 4);
	drawSprite(&enemyAttackSprite, enemyAttackSpritePlayer, 85);

	// Draw background image
	err = drawCompressedImage(&battleBackgroundImage, 0, graphicsHeight);

	// Draw button bar image
	err |= drawCompressedImage(&battleButtonsImage, graphicsHeight+1, 10);

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

	// Draw enemy HP bar
	drawEnemyHpBar();
	if (err != Z_OK) {
		printStringAtXY("Image error.", 1, 2);
	}

	// Draw enemy character image
	drawEnemyCharaImage(steveFace);

	// Set the background color gradient
	unpackbits(unpackedGradient, battleGradient);
	setDliColorTable(unpackedGradient);

	// Switch to final color table and enable DLI
	loadColorTable(battleColorTable);
	POKE(NMIEN, 0xC0);

	// Copy first color to background color shadow register so that the color extends to overscan.
	POKE(COLOR4, unpackedGradient[0]);  

	debugPrint("Init:", SHORT_CLOCK - startTime, 30, 3);
}

