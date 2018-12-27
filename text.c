// text.c

#include "text.h"
#include "graphics.h"
#include "string.h"
//#include "atari_memmap.h"
#include <atari.h>


// Constants
#define TEXTBOX_WIDTH (40)
#define TEXTBOX_HEIGHT (7)



void printCharaStats(UInt8 player, const UInt8 *name, UInt8 level, UInt8 hp, UInt8 maxHp) {
	UInt8 x = (player % 2) * 10 + 2;
	UInt8 y = (player / 2) * 4;
	UInt8 s1[9], s2[6];

	printString(name, x, y);

	printString("Lv", x, y+1);
	numberString(s1, 0, level);
	printString(s1, x + 2, y+1);

	numberString(s1, 0, hp);
	numberString(s2, 0, maxHp);
	appendString(s1, "/");
	appendString(s1, s2);
	printString(s1, x, y+2);
}


void printPartyStats(SInt32 money, UInt16 potions, UInt16 fangs, SInt16 reputation) {
	UInt8 s[16];
	UInt8 len;
	const UInt8 x = 38;
	UInt8 y = 1;

	s[0] = '$';
	numberString(s+1, ',', money);
	len = strlen(s);
	printString(s, x-len-1, y);
	y += 2;

	numberString(s, 0, potions);
	appendString(s, "{");
	len = strlen(s);
	printString(s, x-len, y);
	y += 2;

	numberString(s, ',', fangs);
	appendString(s, "}");
	len = strlen(s);
	printString(s, x-len, y);
	y += 2;

}


void clearTextWindow(void) {
	UInt16 i;

	for (i=0; i<TEXTBOX_HEIGHT*TEXTBOX_WIDTH; ++i) {
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
		c = toAtascii(c);
		textWindow[x + TEXTBOX_WIDTH * y] = c;
		++x;
		++index;
	}
}


void printStringWithLayout(const UInt8 *s, UInt8 top, UInt8 firstIndent, UInt8 leftMargin, UInt8 rightMargin) {
	// firstIndent is independent of leftMargin
	// leftMargin controls lines after the first one
	// rightMargin controls all lines
	// text will be broken at space characters

	UInt8 i = 0;
	UInt8 y = top;
	UInt8 x = firstIndent;
	UInt8 previousBreakable = 0;
	UInt8 lineStartIndex = 0;
	UInt8 xMax = TEXTBOX_WIDTH - rightMargin;
	UInt8 c;

	while (y < TEXTBOX_HEIGHT) {
		c = s[i];
		if (c == ' ' || c == '\n') { // whitespace
			previousBreakable = i;
		}

		if (x >= xMax || c == '\n') {
			// Rewind to previous breakable character
			if (previousBreakable > lineStartIndex) {
				while (i > previousBreakable + 1) {
					--i;
					--x;
					textWindow[x + TEXTBOX_WIDTH * y] = 0; // space char
				}			
			}

			// Skip past whitespace
			while (s[i] == ' ' || s[i] == '\n') {
				++i;
			}
			c = s[i];

			// Start new line
			++y;
			x = leftMargin;
			lineStartIndex = i;
			previousBreakable = i;
		}

		if (c == 0) { return; } // End of string

		textWindow[x + TEXTBOX_WIDTH * y] = toAtascii(c);
		++x;
		++i;
		c = s[i];
	}
}


void printDecimal16bitValue(const UInt8 *label, SInt16 value, UInt8 x, UInt8 y) {
	UInt8 s[7];
	UInt8 labelLength = strlen(label);	
	numberString(s, 0, value);
	printString(label, x, y);
	printString(s, x + labelLength, y);
}


void printHex16bitValue(const UInt8 *label, UInt16 value, UInt8 x, UInt8 y) {
	// Prints a label and a hex value in the text box area.
	UInt8 hexStr[5];
	UInt8 labelLength = strlen(label);
	
	hexString(hexStr, 4, value);
	printString(label, x, y);
	printString(hexStr, x + labelLength, y);
}


void printHex8bitValue(const UInt8 *label, UInt8 value, UInt8 x, UInt8 y) {
	// Prints a label and a hex value in the text box area.
	UInt8 hexStr[3];
	UInt8 labelLength = strlen(label);
	
	hexString(hexStr, 2, value);
	printString(label, x, y);
	printString(hexStr, x + labelLength, y);
}


// Obsolete


void printAllTiles(void) {
	UInt8 i;
	for (i=0; i<40*5; ++i) {
		textWindow[i] = i;
	}
}


