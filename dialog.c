// dialog.c 

#include "dialog.h"
#include "cursor.h"
#include "graphics.h"
#include "menu.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"


#define GRAPHICS_HEIGHT (48)
#define TEXT_HEIGHT (9)

// Constants 


const UInt8 dialogColorTable[] = {
	0x58, // cursor sprite
	0x00, 0x00, 0x00, // unused sprites
	0x32, 0x0E, 0x62, // graphics playfield
	0x68, // missile sprites
	0x90, // graphics background
	0x0E, // text color
	0x00, // text background
	0x34  // hp bar background
};

const DataBlock mobyDickImage = {
	196, { // length
		0x78,0xDA,0xED,0xD3,0xB1,0x0A,0xC2,0x30,0x10,0x06,0xE0,0x40,0x67,0xDB,0xF7,0xC9,
		0xD0,0x3E,0x45,0xE9,0xBB,0x09,0x82,0xA3,0x5E,0x8A,0x75,0xEE,0x92,0x3C,0x47,0xC5,
		0x17,0xC8,0xE4,0xE2,0x52,0xAA,0x31,0x36,0xA1,0xE0,0x90,0xE4,0x1F,0x1C,0x8A,0xF8,
		0x2F,0x59,0x3E,0xEE,0x20,0x77,0x67,0xCC,0x3F,0xAB,0xC8,0x03,0x74,0x83,0xC4,0x1C,
		0xED,0x40,0xB7,0xC5,0xDC,0x71,0x80,0x58,0x73,0xBD,0x40,0xEE,0x5E,0x37,0xF3,0xCB,
		0x12,0xD9,0xD4,0x0D,0x43,0x92,0xA5,0x4B,0xFD,0x68,0x0A,0xD0,0x71,0x05,0xBA,0x33,
		0xE8,0x3A,0xD7,0xDE,0x0F,0xF4,0x16,0x74,0x34,0xB1,0xCA,0x98,0x91,0x5C,0x64,0xD8,
		0x09,0xA3,0x69,0x89,0x08,0xB9,0x92,0x3E,0x12,0x74,0xC5,0x88,0xB9,0x0C,0xAD,0x07,
		0xBA,0x9C,0xE4,0x13,0x71,0xA5,0xB0,0xBF,0xE6,0xE5,0x3E,0xE2,0x78,0xA7,0xEC,0x1F,
		0xEB,0x64,0x3D,0x37,0x0F,0x0B,0x4F,0x56,0x45,0x2E,0x8B,0x4F,0xEE,0x5E,0x74,0xDF,
		0xBF,0xEB,0xB5,0x21,0x57,0x29,0x3F,0x5F,0x3D,0xF7,0x4E,0x6E,0x45,0x4E,0xFA,0x10,
		0xDB,0x83,0xA5,0xBF,0xC0,0x4E,0x95,0xB7,0xDF,0xDD,0xD7,0x4A,0xAD,0xF1,0x7A,0x5F,
		0x08,0x16,0x09,0x26
	}
};

const UInt8 jabberwocky1[] = "'Twas brillig, and the slithy toves\nDid gyre and gimble in the wabe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.";
const UInt8 jabberwocky2[] = "\"Beware the Jabberwock, my son! The jaws that bite, the claws that catch! Beware the Jubjub bird, and shun The frumious Bandersnatch!\"";
const UInt8 mobyDick[] = "\tA word concerning an incident in the last chapter.\n\tAccording to the invariable usage of the fishery, the whale-boat pushes off from the ship, with the headsman or whale-killer as temporary steersman, and the harpooneer or whale-fastener pulling the foremost oar, the one known as the harpooneer-oar. Now it needs a strong, nervous arm to strike the first iron into the fish; for often, in what is called a long dart, the heavy implement has to be flung to the distance of twenty or thirty feet. But however prolonged and exhausting the chase, the harpooneer is expected to pull his oar meanwhile to the uttermost; indeed, he is expected to set an example of superhuman activity to the rest, not only by incredible rowing, but by repeated loud and intrepid exclamations; and what it is to keep shouting at the top of one's compass, while all the other muscles are strained and half started--what that is none know but those who have tried it. For one, I cannot bawl very heartily and work very recklessly at one and the same time. In this straining, bawling state, then, with his back to the fish, all at once the exhausted harpooneer hears the exciting cry--\"Stand up, and give it to him!\" He now has to drop and secure his oar, turn round on his centre half way, seize his harpoon from the crotch, and with what little strength may remain, he essays to pitch it somehow into the whale. No wonder, taking the whole fleet of whalemen in a body, that out of fifty fair chances for a dart, not five are successful; no wonder that so many hapless harpooneers are madly cursed and disrated; no wonder that some of them actually burst their blood-vessels in the boat; no wonder that some sperm whalemen are absent four years with four barrels; no wonder that to many ship owners, whaling is but a losing concern; for it is the harpooneer that makes the voyage, and if you take the breath out of his body how can you expect to find it there when most wanted!";


// Globals 
const UInt8* nextText;


static void printNextText(void) {
	UInt16 oldTextWindow = PEEKW(TXTMSC);
	UInt8 *rowPtr = GRAPHICS_WINDOW + (GRAPHICS_HEIGHT) * SCREEN_ROW_BYTES;

	// Change TXTMSC temporarily
	POKEW(TXTMSC, (UInt16)rowPtr);

	// Clear text box
	zeroOut16(rowPtr, TEXT_HEIGHT*SCREEN_ROW_BYTES);

	// Print text 
	POKE(LMARGN, 0); 		// left margin
	POKE(COLCRS, 0); 		// first indent
	POKE(ROWCRS, 0); 		// row
	POKE(RMARGN, 40); 		// right margin
	POKE(ROWINC, 1);
	nextText = drawTextBox(nextText);

	// Restore TXTMSC
	POKEW(TXTMSC, oldTextWindow);
}

static SInt8 infoCursorHandler(UInt8 event) {
	if (event == CursorClick) {
		if (*nextText == 0) {
			return MessageReturnToMap;
		} else {
			printNextText();
		}
	}

	return MessageNone;
}

void initDialog(void) {
	UInt8 *rowPtr = GRAPHICS_WINDOW + GRAPHICS_HEIGHT * SCREEN_ROW_BYTES;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	setPlayerCursorVisible(0);
	setCursorEventHandler(infoCursorHandler);
	setScreenMode(ScreenModeDialog);
	zeroOut16(GRAPHICS_WINDOW, (GRAPHICS_HEIGHT + TEXT_HEIGHT)*SCREEN_ROW_BYTES);

	// Init text 
	nextText = mobyDick;
	printNextText();

	// Show screen
	fadeInColorTable(dialogColorTable);

	// Draw image
	drawCompressedImage(&mobyDickImage, 0, GRAPHICS_HEIGHT);

}
