// text.c

#include "text.h"
#include "graphics.h"
#include "string.h"
#include "tiles.h"
#include "atari_memmap.h"
#include <atari.h>


// Constants
#define SCREEN_WIDTH (40)
#define TEXT_LUM ((UInt8 *)0x0603)
#define TEXT_BG ((UInt8 *)0x0604)


// == printCharaStats() ==
void printCharaStats(UInt8 player, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp) {
	UInt8 x = player * 9 + 3;
	UInt8 s1[9], s2[6];

	printString(name, x, 0);

	printString("Lv", x, 1);
	numberString(s1, 0, level);
	printString(s1, x + 2, 1);

	numberString(s1, 0, hp);
	numberString(s2, 0, maxHp);
	appendString(s1, "/");
	appendString(s1, s2);
	printString(s1, x, 2);
}

// == printPartyStats() ==
void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs, SInt16 reputation) {
	UInt8 s[16];
	UInt8 len;

	s[0] = '$';
	numberString(s+1, ',', money);
	len = strlen(s);
	printString(s, 12-len, 4);

	numberString(s, 0, potions);
	appendString(s, "{");
	len = strlen(s);
	printString(s, 21-len, 4);

	numberString(s, ',', fangs);
	appendString(s, "}");
	len = strlen(s);
	printString(s, 30-len, 4);

	numberString(s, ',', reputation);
	appendString(s, "rep");
	len = strlen(s);
	printString(s, 40-len, 4);
}


void clearTextWindow(void) {
	UInt16 i;

	for (i=0; i<5*SCREEN_WIDTH; ++i) {
		textWindow[i] = 0;
	}
}


void setTextWindowColorTheme(UInt8 theme) {
	switch (theme) {
		case 1:
			*TEXT_LUM = 0x0E;    // white
			*TEXT_BG  = 0x00;    // black
			break;
		case 2:
			*TEXT_LUM = 0x0E;    // white
			*TEXT_BG  = 0x92;    // dark blue
			break;
		default:
			*TEXT_LUM = 0x0E;    // white
			*TEXT_BG  = 0x04;    // gray
			break;
	}
}


void printString(const UInt8 *s, UInt8 x, UInt8 y) {
	UInt8 index = 0;
	UInt8 c;

	while (c = s[index]) {
		if (c < 0x20) {
			c += 0x40;
		} else if (c < 0x60) {
			c -= 0x20;
		}
		textWindow[(UInt16)x + SCREEN_WIDTH * (UInt16)y] = c;
		++x;
		++index;
	}
}


// == printDebugInfo() ==
void printDebugInfo(const UInt8 *label, UInt16 value, UInt8 position) {
	// Prints a label and a hex value in the text box area.
	UInt8 hexStr[5];
	UInt8 labelLength = strlen(label);
	
	hexString(hexStr, value);
	printString(label, position, 0);
	printString(hexStr, position + labelLength, 0);
}


// Obsolete


void printColorString(const UInt8 *s, UInt8 color, UInt8 x, UInt8 y) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 index = 0;
	UInt8 c;
	
	while (c = s[index]) {
		if (c < 0x20) {
			c += 0x40;
		} else if (c < 0x60) {
			c -= 0x20;
		}
		c += (color * 64);
		screen[(UInt16)x + SCREEN_WIDTH * (UInt16)y] = c;
		++x;
		++index;
	}
}


void printAllTiles(void) {
	UInt8 i;
	for (i=0; i<40*5; ++i) {
		textWindow[i] = i;
	}
}


