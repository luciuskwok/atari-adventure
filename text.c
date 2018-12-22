// text.c

#include "text.h"
#include "graphics.h"
#include "string.h"
//#include "atari_memmap.h"
#include <atari.h>


// Constants
#define TEXTBOX_WIDTH (40)
#define TEXTBOX_HEIGHT (5)
#define TEXT_LUM ((UInt8 *)0x0603)
#define TEXT_BG ((UInt8 *)0x0604)



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
	UInt8 c = s[0];

	while (c != 0 && y < TEXTBOX_HEIGHT) {
		if (c == ' ') { // space char
			previousBreakable = i;
		}
		if (x >= xMax && previousBreakable > lineStartIndex) {
			// Rewind to previous breakable character
			while (i > previousBreakable) {
				--i;
				--x;
				textWindow[x + TEXTBOX_WIDTH * y] = 0; // space char
			}			
			// Fast-forward past spaces
			while (s[i] == ' ') {
				if (s[i] == 0) {
					return;
				}
				++i;
			}
			c = s[i];

			// Start new line
			++y;
			x = leftMargin;
			lineStartIndex = i;
			previousBreakable = i;
		}

		textWindow[x + TEXTBOX_WIDTH * y] = toAtascii(c);
		++x;
		++i;
		c = s[i];
	}
}



void printDebugInfo(const UInt8 *label, UInt16 value, UInt8 position) {
	// Prints a label and a hex value in the text box area.
	UInt8 hexStr[5];
	UInt8 labelLength = strlen(label);
	
	hexString(hexStr, value);
	printString(label, position, 0);
	printString(hexStr, position + labelLength, 0);
}


// Obsolete


void printAllTiles(void) {
	UInt8 i;
	for (i=0; i<40*5; ++i) {
		textWindow[i] = i;
	}
}


