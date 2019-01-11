// text.c

#include "text.h"
#include "atari_memmap.h"
#include "game_chara.h"
#include "graphics.h"
#include <atari.h>
#include <string.h>


// Constants
#define TEXTBOX_HEIGHT (7)


static void printCharaStats(GameCharaPtr chara) {
	UInt8 hp = chara->hp;
	UInt8 maxHp = charaMaxHp(chara);
	UInt8 lvStr[9] = "Lv ";
	UInt8 hpStr[9];

	printLine(chara->name);

	uint8toString(lvStr+3, chara->level);
	printLine(lvStr);

	uint8toString(hpStr, hp);
	stringConcat(hpStr, "/");
	uint8toString(hpStr+stringLength(hpStr), maxHp);
	printLine(hpStr);

	// Draw HP bar
	POKEW(SAVADR, PEEKW(TXTMSC) + PEEK(ROWCRS) * SCREEN_ROW_BYTES);
	// Multiple COLCRS by 4 to switch from text to raster coordinates
	POKE(COLCRS, PEEK(COLCRS) << 2);
	sizeBarChart(hp, maxHp);
	drawBarChart();
}

void printCharaAtIndex(UInt8 index) {
	UInt8 x = 1 + index * 10;
	GameCharaPtr chara = charaAtIndex(index);

	POKE(COLCRS, x);
	POKE(LMARGN, x);
	printCharaStats(chara);
}

void printAllCharaText(UInt8 y) {
	UInt8 count = numberInParty();
	UInt8 i;

	for (i=0; i<count; ++i) {
		POKE(ROWCRS, y);
		printCharaAtIndex(i);
	}
}

/*
void hexString(UInt8 *outString, UInt8 length, UInt16 value) {
	UInt8 i, c;	
	for (i=0; i<length; ++i) {
		c = value & 0x0F;
		if (c < 10) {
			c += 0x30;
		} else {
			c += 0x41 - 10;
		}
		outString[length-1-i] = c;
		value = value >> 4;
	}
	outString[length] = 0;
}
*/
