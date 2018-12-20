// text.c

#include "text.h"
#include "graphics.h"
#include "tiles.h"
#include "atari_memmap.h"
#include <atari.h>


// Constants
#define SCREEN_WIDTH (40)


// == printCharaStats() ==
void printCharaStats(UInt8 player, const char *name, UInt8 level, UInt8 hp) {
	UInt8 x = player * 10;
	char s[8];

	printString(name, x, 0);
	++x;
	printString("Lv:", x, 1);
	printString("HP:", x, 2);
	// TODO: print numbers
}

// == printPartyStats() ==
void printPartyStats(UInt16 money, UInt16 potions, UInt16 fangs, UInt16 reputation) {

}

// == clearTextWindow() ==
void clearTextWindow(void) {
	UInt16 i;

	for (i=0; i<5*SCREEN_WIDTH; ++i) {
		textWindow[i] = 0;
	}
}


// == printColorString() ==
void printColorString(const char *s, UInt8 color, UInt8 x, UInt8 y) {
	UInt8 *screen = (UInt8 *)PEEKW(SAVMSC);
	UInt8 index = 0;
	char c;
	
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

// == printString() ==
void printString(const char *s, UInt8 x, UInt8 y) {
	UInt8 index = 0;
	char c;

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
void printDebugInfo(const char *label, UInt16 value, UInt8 position) {
	// Prints a label and a hex value in the text box area.
	char hexStr[5];
	char labelLength = strlen(label);
	
	hexString(hexStr, value);
	printString(label, position, 0);
	printString(hexStr, position + labelLength, 0);
}

// == hexString() ==
void hexString(char *s, UInt16 x) {
	char i;
	char c;
	
	for (i=0; i<4; ++i) {
		c = x & 0x0F;
		if (c < 10) {
			c += 0x30;
		} else {
			c += 0x41 - 10;
		}
		s[3-i] = c;
		x = x >> 4;
	}
	s[4] = 0;
}

// == strlen() ==
UInt8 strlen(const char *s) {
	char len = 0;
	while (s[len] != 0 && len < 0xFF) {
		++len;
	}
	return len;
}
