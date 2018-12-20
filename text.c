// text.c

#include "text.h"
#include "graphics.h"
#include "tiles.h"
#include "atari_memmap.h"
#include <atari.h>


// Constants
#define SCREEN_WIDTH (40)


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

// == clearTextWindow() ==
void clearTextWindow(void) {
	UInt16 i;

	for (i=0; i<5*SCREEN_WIDTH; ++i) {
		textWindow[i] = 0;
	}
}


// == printColorString() ==
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

// == printString() ==
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

void numberString(UInt8 *outString, UInt8 thousandsSeparator, SInt32 value) {
	UInt8 isNegative = 0;
	UInt8 len = 0;
	UInt8 c, i, j;


	if (value == 0) { // Special case for value of zero.
		outString[0] = '0';
		++len;
	} else {
		if (value < 0) {
			value = -value;
			isNegative = 1;
		}

		// Add digits in reverse order, then reverse the string.
		while (value != 0) {
			if ((len >= 3) && (len % 4 == 3) && (thousandsSeparator != 0)) {
				outString[len] = thousandsSeparator;
				++len;
			}
			c = value % 10;
			outString[len] = c + 0x30;
			++len;
			value = value / 10;
		}

		// Add negative sign
		if (isNegative) {
			outString[len] = '-';
			++len;
		}

		// Reverse the string
		for (i=0, j=len-1; i<j; ++i, --j) {
			c = outString[j];
			outString[j] = outString[i];
			outString[i] = c;
		}
	}
	outString[len] = 0; // null terminate string
}

// == printAllTiles() ==
void printAllTiles(void) {
	UInt8 i;
	for (i=0; i<40*5; ++i) {
		textWindow[i] = i;
	}
}

// == hexString() ==
void hexString(UInt8 *outString, UInt16 value) {
	UInt8 i, c;	
	for (i=0; i<4; ++i) {
		c = value & 0x0F;
		if (c < 10) {
			c += 0x30;
		} else {
			c += 0x41 - 10;
		}
		outString[3-i] = c;
		value = value >> 4;
	}
	outString[4] = 0;
}

// == strlen() ==
UInt8 strlen(const UInt8 *s) {
	UInt8 len = 0;
	while (s[len] != 0 && len < 0xFF) {
		++len;
	}
	return len;
}

// == appendString() ==
void appendString(UInt8 *ioString, const UInt8 *append) {
	UInt8 ai = 0;
	UInt8 i = 0;

	while (ioString[i] != 0 && i < 255) {
		++i;
	}
	while (append[ai] != 0 && i < 255) {
		ioString[i] = append[ai];
		++i;
		++ai;
	}
	ioString[i] = 0; // null terminator
}
