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


const UInt8 dialogLoadingColorTable[] = {
	0x00, 0x00, 0x00, 0x00, // unused, player sprites hidden
	0x00, 0x0E, 0x00, // playfield
	0x06, // missile sprites
	0x00, // background
	0x00, 0x00, 0x00 // unused, DLI disabled
};

const UInt8 dialogColorTable[] = {
	0x58, // cursor sprite
	0x00, 0x00, 0x00, // unused sprites
	0x32, 0x0E, 0x00, // graphics playfield
	0x68, // missile sprites
	0x00, // graphics background
	0x0E, // text color
	0x00, // text background
	0x34  // hp bar background
};

const DataBlock mobyDickImage = {
	197, { // length
		0x78,0xDA,0xED,0xD3,0x31,0x0A,0xC2,0x30,0x14,0x06,0xE0,0x48,0x67,0xCD,0xDE,0xA3,
		0x64,0x68,0x4E,0x21,0xBD,0x8E,0x47,0x70,0x76,0xEA,0x6A,0x02,0xC6,0xB9,0x14,0xEC,
		0x3D,0xC4,0x0B,0x64,0xCA,0xE2,0x52,0x82,0xCF,0xD4,0x94,0x82,0x43,0x9A,0x7F,0x2C,
		0xE8,0x3F,0x7F,0xBC,0x47,0xF2,0xDE,0x63,0xEC,0x9F,0x55,0x64,0x03,0xBA,0xF2,0x80,
		0x39,0x75,0x04,0xDD,0x09,0x73,0xE7,0x12,0x62,0xF5,0xE3,0x0E,0xB9,0xE7,0xBE,0x86,
		0xDC,0x16,0x74,0x05,0xD1,0x8F,0x2E,0x0E,0x07,0x9D,0xE8,0x41,0x77,0x05,0x9D,0x89,
		0xED,0x29,0x26,0x59,0x5D,0x28,0xCF,0x24,0xD1,0xA0,0x62,0xBA,0xB4,0xD3,0x64,0xD5,
		0x1C,0x9D,0x72,0x95,0xFA,0x4A,0xD2,0xF1,0x01,0x73,0x05,0x5A,0x0F,0x74,0x3B,0x75,
		0x7B,0x21,0xAE,0xD2,0xE1,0xD7,0x26,0xD9,0x2C,0x38,0x61,0xFA,0xF0,0x7F,0x36,0x5B,
		0xEF,0x33,0x8F,0x22,0xC0,0x4B,0x50,0x94,0x3E,0x07,0xE1,0xE3,0xBD,0xD8,0xB6,0x1D,
		0xEB,0x99,0x94,0x93,0x6E,0x9A,0xAF,0xB5,0x8B,0x73,0xE3,0xF3,0xBB,0xED,0xD8,0xD6,
		0x65,0xF7,0x26,0xBC,0xDB,0xE3,0xFB,0x95,0x77,0x1D,0xE6,0xA4,0x5B,0xE3,0xF5,0xBE,
		0x01,0xE9,0x65,0x98,0x2E,
	}
};

const UInt8 mobyDickGradient[] = {
	0x00, 0x80, 0xFC, 0x00, 0x00, 0x80, 0xFE, 0x00, 0x06, 0x80, 0x00, 0x80, 0x72, 
	0x64, 0x56, 0x90, 0xFE, 0x80, 0x04, 0x90, 0x80, 0x90, 0x90, 0x80, 0xFD, 0x90, 
	0x00, 0x80, 0xEF, 0x90,
	0x80 // terminator
};

// const UInt8 jabberwocky1[] = "'Twas brillig, and the slithy toves\nDid gyre and gimble in the wabe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.";
// const UInt8 jabberwocky2[] = "\"Beware the Jabberwock, my son! The jaws that bite, the claws that catch! Beware the Jubjub bird, and shun The frumious Bandersnatch!\"";
const UInt8 mobyDickCh62[] = "A word concerning an incident in the last chapter.\n\tAccording to the invariable usage of the fishery, the whale-boat pushes off from the ship, with the headsman or whale-killer as temporary steersman, and the harpooneer or whale-fastener pulling the foremost oar, the one known as the harpooneer-oar. Now it needs a strong, nervous arm to strike the first iron into the fish; for often, in what is called a long dart, the heavy implement has to be flung to the distance of twenty or thirty feet. But however prolonged and exhausting the chase, the harpooneer is expected to pull his oar meanwhile to the uttermost; indeed, he is expected to set an example of superhuman activity to the rest, not only by incredible rowing, but by repeated loud and intrepid exclamations; and what it is to keep shouting at the top of one's compass, while all the other muscles are strained and half started--what that is none know but those who have tried it. For one, I cannot bawl very heartily and work very recklessly at one and the same time. In this straining, bawling state, then, with his back to the fish, all at once the exhausted harpooneer hears the exciting cry--\"Stand up, and give it to him!\" He now has to drop and secure his oar, turn round on his centre half way, seize his harpoon from the crotch, and with what little strength may remain, he essays to pitch it somehow into the whale. No wonder, taking the whole fleet of whalemen in a body, that out of fifty fair chances for a dart, not five are successful; no wonder that so many hapless harpooneers are madly cursed and disrated; no wonder that some of them actually burst their blood-vessels in the boat; no wonder that some sperm whalemen are absent four years with four barrels; no wonder that to many ship owners, whaling is but a losing concern; for it is the harpooneer that makes the voyage, and if you take the breath out of his body how can you expect to find it there when most wanted!";
const UInt8 mobyDickCh58[] = "Steering north-eastward from the Crozetts, we fell in with vast meadows of brit, the minute, yellow substance, upon which the Right Whale largely feeds. For leagues and leagues it undulated round us, so that we seemed to be sailing through boundless fields of ripe and golden wheat.\n\tOn the second day, numbers of Right Whales were seen, who, secure from the attack of a Sperm Whaler like the Pequod, with open jaws sluggishly swam through the brit, which, adhering to the fringing fibres of that wondrous Venetian blind in their mouths, was in that manner separated from the water that escaped at the lip.\n\tAs morning mowers, who side by side slowly and seethingly advance their scythes through the long wet grass of marshy meads; even so these monsters swam, making a strange, grassy, cutting sound; and leaving behind them endless swaths of blue upon the yellow sea.*";

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
	// Use block of memory at end of screen memory area for gradient.
	UInt8 *unpackedGradient = GRAPHICS_WINDOW + (GRAPHICS_HEIGHT + TEXT_HEIGHT) * SCREEN_ROW_BYTES;
	UInt8 *rowPtr = GRAPHICS_WINDOW + GRAPHICS_HEIGHT * SCREEN_ROW_BYTES;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	setPlayerCursorVisible(0);
	setCursorEventHandler(infoCursorHandler);
	zeroOut16(GRAPHICS_WINDOW, (GRAPHICS_HEIGHT + TEXT_HEIGHT)*SCREEN_ROW_BYTES);

	// Set loading color table and disable DLI
	setScreenMode(ScreenModeDialog);
	POKE(NMIEN, 0x40);

	// Init text 
	nextText = mobyDickCh58;
	printNextText();

	// Show screen
	fadeInColorTable(dialogLoadingColorTable);

	// Set the background color gradient
	unpackbits(unpackedGradient, mobyDickGradient);
	setDliColorTable(unpackedGradient);

	// Draw image
	drawCompressedImage(&mobyDickImage, 0, GRAPHICS_HEIGHT);

	// Switch to final color table and enable DLI
	loadColorTable(dialogColorTable);
	POKE(NMIEN, 0xC0);

}
