// text.c

#include "text.h"
#include "atari_memmap.h"
#include "game_chara.h"
#include "graphics.h"
#include <atari.h>
#include <string.h>


// Constants
#define TEXTBOX_HEIGHT (7)



static void drawHpBar(UInt8 hp, UInt8 maxHp) {
	UInt8 width;
	UInt8 fill;
	UInt8 remainder;
	UInt16 n;

	// Multiple COLCRS by 4 to switch from text to raster coordinates
	POKE(COLCRS, PEEK(COLCRS) << 2);

	if (maxHp >= 72) {
		width = 36;
	} else {
		width = maxHp / 2;
	}
	n = (UInt16)hp * width;
	remainder = n % maxHp;
	fill = n / maxHp;
	if (remainder) {
		++fill;
	}
	drawBarChart(textWindow, width, fill);
}

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

	drawHpBar(hp, maxHp);
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

void printPartyStats(void) {
	UInt8 s[40];
	UInt8 len;
	UInt8 x;

	s[0] = '$';
	uint16toString(s+1, partyMoney);
	stringConcat(s, "  ");

	uint8toString(s+stringLength(s), partyPotions);
	stringConcat(s, "\x11  ");

	uint8toString(s+stringLength(s), partyFangs);
	stringConcat(s, "\x12");

	len = stringLength(s);
	x = 20 - (len >> 1);

	SET_TXT_ORIGIN(x, TEXTBOX_HEIGHT-2)
	printLine(s);
}

void drawTextBox(const UInt8 *s) {
	// On entry:
	// COLCRS: starting & newline X-position.
	// ROWCRS: starting Y-position.
	// LMARGN: X-position after wrapping line
	// RMARGN: X-position of right margin
	// ROWINC: line spacing

	UInt8 x = PEEK(COLCRS);
	UInt8 y = PEEK(ROWCRS);
	UInt8 newlineMargin = x;
	UInt8 wrapMargin = PEEK(LMARGN);
	UInt8 xMax = PEEK(RMARGN);
	UInt8 lineSpacing = PEEK(ROWINC);

	UInt8 i = 0;
	UInt8 previousBreakable = 0;
	UInt8 lineStartIndex = 0;
	UInt8 isNewLine = 1;
	UInt8 c;

	while (y < TEXTBOX_HEIGHT) {
		c = s[i];
		if (c == ' ' || c == '\n') { // whitespace
			previousBreakable = i;			
		}

		if (x >= xMax || c == '\n') {
			isNewLine = 0;

			// Rewind to previous breakable character
			if (previousBreakable > lineStartIndex) {
				while (i > previousBreakable + 1) {
					--i;
					--x;
					textWindow[x + SCREEN_ROW_BYTES * y] = 0; // space char
				}			
			}

			if (c == '\n') {
				isNewLine = 1;
			}

			// Skip past whitespace
			while (s[i] == ' ' || s[i] == '\n') {
				++i;
				if (s[i] == '\n') {
					y += lineSpacing;
					isNewLine = 1;
				}
			}
			c = s[i];

			// Start new line
			y += lineSpacing;
			if (isNewLine != 0) {
				x = newlineMargin;
			} else {
				x = wrapMargin;
			}
			lineStartIndex = i;
			previousBreakable = i;
		}

		if (c == 0) { return; } // End of string

		textWindow[x + SCREEN_ROW_BYTES * y] = toAtascii(c);
		++x;
		++i;
		c = s[i];
	}
}

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

void debugPrint(const UInt8 *s, UInt16 value, UInt8 x, UInt8 y) {
	UInt8 message[40];

	stringCopy(message, s);
	uint16toString(message+stringLength(message), value);
	printStringAtXY(message, x, y);
}
