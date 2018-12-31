// info.c

#include "info.h"
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "sprites.h"
#include "text.h"
#include <string.h>


static SInt8 infoCursorHandler(UInt8 event) {
	return (event == CursorClick) ? MessageReturnToMap : MessageNone;
}

static void drawCharaInfoAtIndex(UInt8 index) {
	GameCharaPtr chara = charaAtIndex(index);
	UInt8 maxHp = charaMaxHp(chara);
	UInt8 hp = chara->hp;
	UInt8 x = 1 + index * 10;
	UInt8 y = 0;
	UInt8 s[11];

	printString(chara->name, x, y++);

	stringCopy(s, "Lv ");
	numberString(s+3, 0, chara->level);
	printString(s, x, y++);
	
	stringCopy(s, "next ");
	numberString(s+5, 0, charaXpToNextLevel(chara));
	printString(s, x, y++);

	++y;

	printString("HP", x, y++);

	numberString(s, 0, hp);
	stringConcat(s, "/");
	numberString(s+stringLength(s), 0, maxHp);
	printString(s, x, y++);
	
	++y;

	stringCopy(s, "ATK ");
	numberString(s+4, 0, charaAttackRating(chara));
	printString(s, x, y++);

	stringCopy(s, "DEF ");
	numberString(s+4, 0, charaDefenseRating(chara));
	printString(s, x, y++);

	++y;

	printString("Sword+1", x, y++);
	printString("Armor+2", x, y++);
	printString("Shield+1", x, y++);
}

void initInfo(void) {
	const UInt8 missileHeight = 91;
	const UInt8 itemWindowRow = 15;
	UInt8 count = numberInParty();
	UInt8 i;
	UInt8 s[20];

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	setPlayerCursorVisible(0);
	clearRasterScreen(16+24); // also clears text window

	// Position missile sprites as borders
	fillSprite(0, 0xFF, 0, missileHeight);
	fillSprite(0, 0x03, missileHeight, 128-missileHeight);
	setSpriteHorizontalPosition(5, 48);
	setSpriteHorizontalPosition(6, 88);
	setSpriteHorizontalPosition(7, 128);
	setSpriteHorizontalPosition(8, 168);

	// Turn on screen
	setScreenMode(ScreenModeInfo);
	fadeInColorTable(FadeTextBox, infoColorTable);

	// Print chara info
	for (i=0; i<count; ++i) {
		drawCharaInfoAtIndex(i);
	}

	// Print party info
	printString("Items", 1, itemWindowRow);

	stringCopy(s, "Gold: $");
	numberString(s+stringLength(s), ',', partyMoney);
	printString(s, 1, itemWindowRow+1);

	stringCopy(s, "Herbs: ");
	numberString(s+stringLength(s), 0, partyPotions);
	stringConcat(s, "{");
	printString(s, 1, itemWindowRow+2);

	stringCopy(s, "Fangs: ");
	numberString(s+stringLength(s), 0, partyFangs);
	stringConcat(s, "}");
	printString(s, 1, itemWindowRow+3);

	stringCopy(s, "Nuts: ");
	numberString(s+stringLength(s), 0, 11);
	printString(s, 16, itemWindowRow+1);

	stringCopy(s, "Staff: ");
	numberString(s+stringLength(s), 0, 3);
	printString(s, 16, itemWindowRow+3);
	
	printString("Boat", 31, itemWindowRow+1);
	printString("Lamp", 31, itemWindowRow+2);
	printString("Crystal", 31, itemWindowRow+3);
	printString("Mantle", 31, itemWindowRow+4);

	registerCursorEventHandler(infoCursorHandler);
}
