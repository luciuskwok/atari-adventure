// info.c

#include "info.h"
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "sprites.h"
#include "text.h"


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
	
	stringCopy(s, "atk ");
	numberString(s+4, 0, charaAttackRating(chara));
	printString(s, x+2, y++);

	stringCopy(s, "def ");
	numberString(s+4, 0, charaDefenseRating(chara));
	printString(s, x+2, y++);
	
	stringCopy(s, "Lv ");
	numberString(s+3, 0, chara->level);
	printString(s, x, y++);
	
	stringCopy(s, "next ");
	numberString(s+5, 0, charaXpToNextLevel(chara));
	printString(s, x, y++);

	printString("HP", x, y++);

	numberString(s, 0, hp);
	stringConcat(s, "/");
	numberString(s+stringLength(s), 0, maxHp);
	printString(s, x, y++);
	
	drawHpBar(x, y++, hp, maxHp);

	printString("Sword", x, y++);
	printString("atk +1", x+2, y++);

	printString("Armor", x, y++);
	printString("def +2", x+2, y++);

	printString("Shield", x, y++);
	printString("def +1", x+2, y++);
}

void initInfo(void) {
	UInt8 count = numberInParty();
	UInt8 i;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	setPlayerCursorVisible(0);
	clearRasterScreen(16+24); // also clears text window

	// Turn on screen
	setScreenMode(ScreenModeInfo);
	fadeInColorTable(FadeTextBox, infoColorTable);

	// Print chara info
	for (i=0; i<count; ++i) {
		drawCharaInfoAtIndex(i);
	}

	// Print party info


	registerCursorEventHandler(infoCursorHandler);
}
