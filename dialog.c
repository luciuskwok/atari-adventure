// dialog.c

#include "dialog.h"
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "menu.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"
#include <zlib.h>


// Constants
#define graphicsHeight (72)

enum NodeTypes {
	MenuNodeTypeA = 0xFF00,
	MenuNodeTypeB = 0xFF02,
	MenuItemNode = 0xFF03,
	BuyItemNode = 0xFF10,
	SellNode = 0xFF20,
	TalkNode = 0xFF30,
	MessageNode = 0xFF31,
	ExitNode = 0xFFFF
};
#define DIALOG_STACK_CAPACITY (16)
#define NODE_CHILD_CAPACITY (8)


// Type
typedef struct tnode *TreeNodePtr;
typedef struct tnode {
	UInt8 *text;
	UInt16 value;
	struct tnode *children[]; 
} TreeNode;

typedef struct StackItem {
	TreeNodePtr node;
	UInt8 selectedRow;
	UInt8 padding;
} StackItem;




// Globals
StackItem dialogStack[DIALOG_STACK_CAPACITY];
UInt8 dialogStackCount = 0;
static UInt8 isExittingDialog;
static UInt8 messageIndex;


// Data
StackItem nullStackItem = { NULL, 0 };

TreeNode exitNode = { "Exit", ExitNode, { NULL } };

TreeNode backNode = { "<Back", ExitNode, { NULL } };

TreeNode buyFlake1InfoNode = { "Heals 2HP\nDISCOUNT FOOD OF TEM!!!", 3, { NULL } };
TreeNode buyFlake1Node = { "tem flake", BuyItemNode, { &buyFlake1InfoNode, NULL } };

TreeNode buyFlake2InfoNode = { "Heals 2HP\nfood of tem", 1, { NULL } };
TreeNode buyFlake2Node = { "tem flake (onsale,)", BuyItemNode, { &buyFlake2InfoNode, NULL } };

TreeNode buyFlake3InfoNode = { "Heals 2HP\nfood of tem\n(expensiv)", 10, { NULL } };
TreeNode buyFlake3Node = { "tem flake (expensiv)", BuyItemNode, { &buyFlake3InfoNode, NULL } };

TreeNode buyCollegeInfoNode = { "COLLEGE\tem pursu higher education", 1000, { NULL } };
TreeNode buyCollegeNode = { "tem pay for colleg", BuyItemNode, { &buyCollegeInfoNode, NULL } };

TreeNode temShopBuyMenuNode = { 
	"hOI!!!\nwelcom to...\nTEM SHOP!",
	MenuNodeTypeB, 
	{
		&buyFlake1Node,
		&buyFlake2Node,
		&buyFlake3Node,
		&buyCollegeNode,
		&backNode,
		NULL
	}
};

TreeNode helloMessage1Node = { "* hOI!!!", MessageNode, { NULL } };
TreeNode helloMessage2Node = { "* i'm temmie", MessageNode, { NULL } };

TreeNode temShopTalkHelloChoiceNode = { 
	"Say Hello", TalkNode, { 
		&helloMessage1Node, 
		&helloMessage2Node, 
		NULL 
	} 
};

TreeNode armorMessage1Node = { "* tem armor so GOOds!\n* any battle becom!\n* a EASY victories!!!", MessageNode, { NULL } };
TreeNode armorMessage2Node = { "* but, hnnn, tem think...\n* if u use armors, battles woudn b a challenge anymores,", MessageNode, { NULL } };
TreeNode armorMessage3Node = { "* but tem...\n* have a solushun!", MessageNode, { NULL } };
TreeNode armorMessage4Node = { "* tem wil offer...\n* a SKOLARSHIPS!", MessageNode, { NULL } };
TreeNode armorMessage5Node = { "* if u lose a lot of battles, tem wil LOWER THE PRICE!", MessageNode, { NULL } };
TreeNode armorMessage6Node = { "* so if you get to TOUGH BATLE and feel FRUSTRATE, can buy TEM armor as last resort!", MessageNode, { NULL } };
TreeNode temShopTalkArmorChoiceNode = { 
	"About Temmie Armor", 
	TalkNode, { 
		&armorMessage1Node, 
		&armorMessage2Node, 
		&armorMessage3Node, 
		&armorMessage4Node, 
		&armorMessage5Node, 
		&armorMessage6Node, 
		NULL 
	} 
};

TreeNode historyMessageNode = { "* tem have DEEP HISTORY!!!", MessageNode, { NULL } };
TreeNode temShopTalkHistoryChoiceNode = { "Temmie History", TalkNode, { &historyMessageNode, NULL } };

TreeNode shopMessageNode = { "* yaYA1!!!\n* go to TEM SHOP!!!", MessageNode, { NULL } };
TreeNode temShopTalkShopChoiceNode = { "About Shop", TalkNode, { &shopMessageNode, NULL } };

TreeNode temShopTalkMenuNode = { 
	"HOI!!!\nim temmie",
	MenuNodeTypeB, 
	{
		&temShopTalkHelloChoiceNode,
		&temShopTalkArmorChoiceNode,
		&temShopTalkHistoryChoiceNode,
		&temShopTalkShopChoiceNode,
		&backNode,
		NULL
	}
};

TreeNode temShopBuyChoiceNode = { "Buy", MenuItemNode, { &temShopBuyMenuNode, NULL } };

TreeNode temShopSellChoiceNode = { "Sell", MenuItemNode, { NULL } };

TreeNode temShopTalkChoiceNode = { "Talk", MenuItemNode, { &temShopTalkMenuNode, NULL } };

TreeNode temShopRootNode = { 
	"* hOi!\n* welcom to...\n* da TEM SHOP!!!", 
	MenuNodeTypeA, 
	{ 
		&temShopBuyChoiceNode, 
		&temShopSellChoiceNode, 
		&temShopTalkChoiceNode, 
		&exitNode, 
		NULL
	} 
};

const UInt8 temShopColorTable[] = {
	0x58, // cursor sprite
	0x00, // unused sprite
	0x00, 0x00, // face sprite
	0x26, 0x0E, 0x00, // playfield
	0xFF, // missile sprites
	0x2A, // background
	0x0E, 0x00, 0x82 // unused, no DLI
};

const DataBlock temShopImage = {
	1156, // length
	{
		0x78,0xDA,0x8D,0xD6,0x41,0x6B,0xE3,0x46,0x14,0x00,0xE0,0x51,0x65,0x5D,0x02,0x59,
		0xD9,0x60,0xFD,0x89,0x9C,0x7A,0x0E,0xA4,0x1E,0x41,0x7C,0xEE,0x18,0xF4,0x64,0x6C,
		0xDA,0xF8,0x94,0x85,0xCD,0x29,0xD0,0x83,0x21,0xDD,0xD6,0x0B,0x3D,0xF7,0x98,0x94,
		0xF4,0xB4,0x97,0x84,0x66,0x9C,0x25,0xBD,0x84,0x9E,0x42,0xD4,0x1F,0x50,0xDA,0x8B,
		0xD8,0x93,0x9A,0x6B,0x71,0x0F,0xEB,0x43,0x8D,0x21,0xD8,0x55,0xDF,0xBC,0x99,0x91,
		0x64,0x25,0x85,0xD5,0xC1,0x89,0xA5,0xCF,0x6F,0xE6,0xBD,0x99,0x79,0x36,0x03,0x06,
		0xEC,0x63,0xAE,0xE8,0xE3,0x9C,0x83,0x8E,0x7B,0x80,0x57,0xA4,0x5E,0x60,0xF4,0x0F,
		0xDD,0xEE,0x24,0x4F,0x9C,0x03,0x3C,0x80,0xE2,0x3A,0xB8,0xF9,0x03,0xEF,0xBA,0xB3,
		0xE7,0x9C,0xA8,0x3A,0x79,0x07,0x9C,0x7D,0x26,0x13,0x07,0x47,0x60,0xAC,0x78,0x42,
		0xCE,0x13,0x4C,0xDD,0x52,0x9F,0x7B,0x21,0xA7,0x29,0xC0,0x4C,0xA6,0xE6,0x79,0xD7,
		0xFC,0x0D,0x85,0xD7,0x13,0x81,0x72,0x3A,0x9F,0x5D,0x29,0x17,0xD0,0xFF,0x69,0xC7,
		0xBA,0xD6,0x86,0xE3,0xC5,0x3C,0x76,0xDF,0xEE,0x5C,0xC3,0xC1,0xDB,0x2D,0x72,0x21,
		0x3A,0x33,0x3D,0x74,0x21,0x0F,0xCA,0xF9,0xEE,0xBE,0xF9,0x64,0x8A,0x2F,0x5B,0x94,
		0x07,0x0F,0x45,0xC8,0x22,0x7A,0xEA,0xF1,0xA0,0xE6,0x18,0x3A,0x46,0x2E,0xE0,0x2D,
		0x01,0x3C,0x0A,0xAD,0xC3,0x5A,0x57,0xDD,0x4E,0xF2,0x97,0x76,0x02,0x5D,0xC0,0xA1,
		0xA1,0xEE,0x07,0x2C,0x70,0x6A,0x6E,0xCB,0x3A,0xCE,0x42,0x4C,0x11,0x84,0x76,0x98,
		0xA6,0xD8,0x70,0x9D,0x3C,0x2F,0x9D,0xD7,0xD3,0x41,0xB0,0x7C,0xCC,0xA9,0xB8,0x17,
		0x6F,0x3A,0x98,0xEA,0xAA,0x74,0x60,0x5D,0xB4,0xE9,0xBE,0x53,0x15,0xE9,0xFF,0xAD,
		0xC7,0x0A,0x85,0x53,0x38,0x8E,0xB9,0x54,0xDC,0xF7,0x54,0xD6,0xAF,0xE9,0x0D,0xE6,
		0xE1,0xE8,0xE9,0x31,0xE1,0xD5,0xDC,0x2D,0x46,0x03,0xF8,0xC2,0x3A,0x66,0x1C,0x47,
		0x54,0x59,0x0E,0xE6,0xA6,0x30,0xFA,0x33,0x35,0xAE,0x89,0x0B,0x0F,0xE6,0x21,0xD6,
		0x18,0xAA,0x0E,0xE0,0xE8,0xF1,0x16,0xFA,0x85,0x0B,0x4A,0x57,0x29,0x9F,0x72,0x52,
		0x5E,0x60,0xC6,0xEA,0x4D,0xA3,0xE2,0x22,0xB5,0xA5,0xCB,0x6B,0x1B,0x62,0x29,0xAF,
		0xF4,0x04,0x3D,0x2C,0x1A,0x5D,0x7A,0xDF,0x7B,0x9B,0x6E,0x48,0xEE,0x58,0x6D,0x34,
		0x30,0x8E,0x53,0xED,0x82,0x8A,0xCB,0xD7,0xCA,0x49,0x80,0x84,0x5C,0xB1,0x9F,0x55,
		0x4D,0x2A,0x69,0xF8,0xF2,0x14,0x8E,0x94,0x1B,0xCD,0xD5,0xD9,0x0A,0x59,0x83,0x06,
		0xD4,0x63,0x17,0xCE,0xC3,0xDD,0x7C,0x8A,0x69,0x48,0x79,0x3A,0xBC,0xC1,0x65,0x89,
		0xBC,0xD2,0xE1,0xC8,0xEA,0x35,0x0A,0x3C,0xDC,0xBC,0xD1,0xAC,0x70,0x12,0xEB,0x1D,
		0x35,0x9C,0x86,0x4E,0xD4,0xC3,0x6A,0x57,0x0E,0x9B,0x72,0x07,0xC6,0x5D,0xB3,0x40,
		0x78,0x3A,0x45,0xB5,0x76,0x58,0xF0,0xF2,0xEA,0xA3,0xF8,0x56,0xB9,0x8B,0x98,0x9C,
		0xAD,0x18,0x66,0x0A,0x50,0x73,0x07,0xC6,0x4D,0x59,0x50,0x6C,0x23,0xED,0xCC,0xE9,
		0x70,0xC8,0x01,0xB9,0x2B,0x50,0xCE,0xE6,0xA7,0xB2,0x00,0xB0,0xD5,0xC3,0x4A,0xBA,
		0x98,0xC6,0x89,0x76,0xF1,0x54,0x85,0x10,0x9E,0x1E,0xA8,0x98,0x9D,0x30,0xEE,0xD6,
		0xBA,0x83,0x69,0x75,0x4A,0xCA,0xF5,0x54,0xF8,0x26,0x39,0x36,0x7B,0x87,0x2D,0x83,
		0xDC,0x49,0xC5,0x09,0x1A,0x3B,0x54,0x2D,0xC4,0xBA,0x69,0x4A,0x6E,0x0A,0x27,0xD7,
		0xAA,0xF0,0xD4,0x4A,0x68,0x9E,0x91,0xE7,0x74,0x23,0xE8,0xB6,0xB5,0xDB,0x95,0xB7,
		0xE4,0x64,0xFE,0x58,0x77,0x0E,0x3A,0xE6,0x35,0xAC,0xBB,0xD7,0xEE,0x97,0x93,0x9F,
		0xEB,0x2E,0x0C,0x05,0x73,0xF6,0xB5,0xF3,0x67,0xC6,0xDD,0x3D,0x26,0x54,0x2A,0x2A,
		0x0B,0xA7,0xED,0xCC,0x7B,0xA2,0x19,0x76,0xB5,0x63,0x9D,0xB5,0x89,0xF7,0x81,0x59,
		0xE7,0x94,0x0E,0xDB,0x96,0x71,0xFE,0xD2,0xC4,0x5B,0x31,0x4B,0xF4,0xE2,0x91,0x6B,
		0xB0,0x9E,0x71,0xDB,0x99,0x89,0x77,0xCC,0xEC,0x90,0xD6,0x85,0x42,0xEC,0x73,0x2C,
		0xA2,0x23,0xFC,0x7C,0xE5,0x8F,0xA9,0xCE,0x72,0x3D,0x37,0x87,0xC7,0x7C,0x1E,0x5D,
		0x37,0x12,0x01,0xB6,0x30,0x7C,0xDF,0xC9,0x57,0xDB,0xD9,0x23,0xB9,0xE5,0x71,0xD9,
		0xE9,0xA9,0xCC,0xD8,0xAF,0xDA,0xF8,0x4F,0xDB,0x38,0x7F,0x4C,0xFB,0x6A,0x9A,0xCD,
		0x37,0xBF,0x15,0x00,0xFB,0x60,0x3B,0xB2,0x0E,0x5E,0xBB,0xD9,0x83,0x72,0x37,0x63,
		0x55,0x16,0x97,0x6A,0xA3,0x77,0x0C,0x3A,0xBD,0xA1,0x85,0x76,0xE3,0xB1,0x4A,0xE4,
		0x2E,0x53,0xC4,0x5F,0x11,0x51,0x5F,0x2F,0x91,0xC7,0xCD,0x42,0x37,0xD1,0xA5,0x18,
		0x2F,0xFB,0x17,0x87,0x5D,0x52,0xBC,0xBD,0x55,0x00,0x42,0x3B,0x6C,0x43,0x6D,0xBD,
		0x1D,0xD1,0xF9,0xE9,0x04,0xE3,0x4D,0x1E,0x65,0x9E,0x51,0xBC,0xC5,0x04,0x77,0x13,
		0x9E,0x75,0x75,0x8A,0xAA,0xCE,0x5D,0x4C,0xDC,0x6C,0x9D,0xE5,0xF9,0x72,0x3C,0xA6,
		0xD6,0x95,0xA3,0xC3,0xC5,0xD7,0xFD,0x7E,0xD3,0x8D,0x7F,0x1F,0xE7,0xF9,0x3A,0x7B,
		0xE8,0xFC,0x0A,0x9F,0xF7,0x57,0x54,0xE5,0x60,0xC3,0xE1,0x24,0xFA,0x38,0x50,0xF6,
		0x1B,0xC5,0x5B,0xCE,0xEE,0xE0,0x9B,0x51,0x42,0xA7,0x06,0xEA,0x0E,0x16,0x93,0xF4,
		0xE1,0x7D,0xAE,0xAE,0xC9,0xFD,0x87,0x64,0x6F,0xC4,0x74,0xCB,0x00,0x72,0x5E,0xE9,
		0xD6,0xA3,0xF4,0xCB,0xF4,0xAB,0x3C,0x5F,0xED,0x8D,0xE4,0x75,0xB2,0x78,0x4D,0x47,
		0x4D,0x47,0x2C,0x5C,0x53,0x15,0x62,0x34,0xF7,0x13,0x17,0xDD,0xA7,0xAE,0xBC,0x49,
		0xD2,0xB9,0x6A,0x79,0x78,0x24,0xBB,0x94,0xAF,0x27,0x0A,0xD7,0x19,0xCD,0x5D,0x72,
		0x78,0x52,0xEE,0xDD,0xF7,0x89,0x6D,0x8D,0xF8,0x47,0x3B,0xEA,0x87,0x11,0xF3,0xFB,
		0xB8,0xAC,0x7E,0x6E,0x2F,0x56,0xB4,0x46,0x72,0x66,0xA6,0xAC,0x8D,0x64,0xD5,0x29,
		0x94,0x8A,0xBA,0xE1,0x78,0x53,0xBF,0x69,0xD8,0x50,0x6B,0xDD,0x6C,0xE6,0x2A,0xB9,
		0xE8,0x59,0xB7,0xCC,0x17,0x65,0x53,0x9A,0xEB,0x22,0x60,0x9A,0x3A,0x8F,0xD2,0xB9,
		0xEB,0x6A,0xF3,0x9A,0xDB,0xF2,0x01,0x39,0x87,0xB7,0x74,0xE7,0xC5,0x33,0x5C,0x77,
		0xCC,0xFC,0xDE,0xE0,0xE4,0x1C,0x5E,0x75,0xFD,0x9C,0x06,0x1F,0xFE,0x30,0xA7,0x33,
		0x6E,0x16,0xE4,0x69,0x3C,0x72,0x83,0xF8,0xDC,0x38,0x1A,0x1C,0x5F,0x84,0x67,0xDB,
		0x66,0x39,0x6E,0x7C,0x39,0x18,0xFC,0x58,0x38,0x3A,0x99,0xCA,0xD9,0x2E,0x4E,0xEE,
		0xE8,0x72,0x08,0xC3,0x78,0xF0,0xEA,0x9C,0xE6,0xE7,0x59,0xE7,0xE0,0xB9,0x80,0xE2,
		0x5B,0x2C,0x87,0xE1,0x61,0x2C,0x71,0xD4,0xA3,0x43,0xE3,0xC4,0xF3,0x2E,0xBE,0x7A,
		0x15,0x5F,0xC2,0xF0,0xEC,0xFC,0xE5,0x59,0xE9,0x5A,0xA2,0xEE,0x3A,0x70,0x38,0x3C,
		0x7B,0x09,0x70,0x16,0x1F,0xC5,0xDA,0x45,0x9C,0x35,0xBA,0xCF,0xB9,0x41,0x1C,0x63,
		0xBC,0x33,0x53,0x3F,0xA7,0xD7,0x72,0x5A,0xC1,0xFF,0x39,0x80,0x53,0xEB,0xA2,0xFD,
		0xFD,0x7D,0xEF,0xD9,0x71,0x2F,0xAA,0xEB,0x81,0x3F,0x0F,0x5B,0x54,0x9C,0xBA,0x1B,
		0x6C,0xAE,0x1B,0x35,0xB5,0xA7,0x2E,0xAF,0xAF,0x2F,0x75,0x36,0xB5,0x5C,0xFF,0x01,
		0x39,0x0C,0xC1,0x01,
	}
};

const DataBlock temFaceSpriteL = {
	24, // length
	{
		0x00, 0x38, 0x44, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x1E, 0x1E, 0x0C, 0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,  
	}
};
const DataBlock temFaceSpriteR = {
	24, // length
	{
		0x1C, 0x22, 0x00, 0x00, 0x00, 0x00, 0x06, 0x0F, 0x0F, 0x06, 0x00, 0x00, 0x00, 0x80, 0x00, 0x04, 0xE4, 0xF8, 0xE0, 0xE0, 0xE0, 0x40, 0x00, 0x00, 
	}
};


// Testing strings:
// UInt8 sansMessage[] = "Sans: Why are graveyards so noisy?\n Because of all the *coffin*!";
// UInt8 ellieMessage[] = "Ellie: How are you doing today? That teacher was totally unfair. C'mon, let's go to the beach!";
// UInt8 papyrusMessage[] = "Papyrus: Nyeh Heh Heh!";


void pushDialogStack(StackItem *item) {
	if (dialogStackCount < DIALOG_STACK_CAPACITY) {
		dialogStack[dialogStackCount].node = item->node;
		dialogStack[dialogStackCount].selectedRow = item->selectedRow;
		++dialogStackCount;
	}
}

void topOfStack(StackItem *outItem) {
	StackItem result = { NULL, 0 };

	if (dialogStackCount > 0) { 
		UInt8 index = dialogStackCount - 1;
		result.node = dialogStack[index].node;
		result.selectedRow = dialogStack[index].selectedRow;
	}
	if (outItem) {
		outItem->node = result.node;
		outItem->selectedRow = result.selectedRow;
	}
}

void popDialogStack(StackItem *outItem) {
	topOfStack(outItem);
	if (dialogStackCount > 0) { 
		--dialogStackCount;
	}
}

void popDialogStackRoot(StackItem *outItem) {
	if (dialogStackCount > 1) {
		dialogStackCount = 1;
	}
	popDialogStack(outItem);
}

UInt8 isStackEmpty(void) {
	return dialogStackCount == 0;
}

void drawVerticalDivider(UInt8 x) {
	UInt8 y;
	
	SET_TXT_ORIGIN(x, 0)

	for (y=0; y<7; ++y) {
		printLine("|");
	}
}

void drawMenu(TreeNodePtr node) {
	const UInt8 topMargin = 18;
	UInt8 x = 0, y = 1;
	TreeNodePtr child;
	UInt8 i;

	if (node->value == MenuNodeTypeA) {
		x = 32;
	} else if (node->value == MenuNodeTypeB) {
		x = 2;
	} else {
		return;
	}

	// Update menu settings
	menuOrigin.x = x * 4;
	menuOrigin.y = (y + topMargin) * 4;
	menuItemCount = 0;

	SET_TXT_ORIGIN(x, y)

	for (i=0; i<6; ++i) {
		child = node->children[i];
		if (child) {
			if (child->value == BuyItemNode) {
				// Get additional info 
				TreeNodePtr info = child->children[0];
				UInt8 s[40] = "$";
				uint16toString(s+1, info->value);
				stringConcat(s, " - ");
				stringConcat(s, child->text);
				printLine(s);
			} else {
				printLine(child->text);
			}
			++menuItemCount;
		} else {
			break;
		}
	}
}

void drawStatus(void) {
	UInt8 s[8] = "$";
	UInt8 len;

	// Draw money and potion count.
	uint16toString(s+1, partyMoney);
	printStringAtXY(s, 28, 6);

	uint8toString(s, partyPotions);
	stringConcat(s, "\x11");
	len = stringLength(s);
	printStringAtXY(s, 40-len, 6);
}


void drawNode(TreeNodePtr node, UInt8 selectIndex) {
	const UInt8 y = 1;

	zeroOut16(TEXT_WINDOW, 7*SCREEN_ROW_BYTES);

	if (node->value == MenuNodeTypeA || node->value == MenuNodeTypeB) {
		drawVerticalDivider(28);

		if (node->value == MenuNodeTypeA) {
			POKE(COLCRS, 4);
			POKE(RMARGN, 4 + 24); 	// right margin
			POKE(ROWINC, 2); 		// line spacing
		} else {
			POKE(COLCRS, 30);
			POKE(RMARGN, 40); 		// right margin
			POKE(ROWINC, 1); 		// line spacing
		}
		POKE(LMARGN, PEEK(COLCRS));
		POKE(ROWCRS, 1); 			// Y-position
		drawTextBox(node->text);
		drawMenu(node);
		setMenuSelectedIndex(selectIndex);
		drawStatus();
	} else if (node->value == TalkNode) {
		TreeNodePtr message = node->children[messageIndex];
		if (message) {
			POKE(COLCRS, 2);
			POKE(LMARGN, 4);
			POKE(ROWCRS, 1); 		// Y-position
			POKE(RMARGN, 34); 		// right margin
			POKE(ROWINC, 2); 		// line spacing
			drawTextBox(message->text);
		}
	}
}

void exitCurrentNode(void) {
	popDialogStack(NULL);

	if (isStackEmpty()) {
		isExittingDialog = 1;
	} else {
		StackItem item;
		topOfStack(&item);

		// Draw previous node & restore selection
		drawNode(item.node, item.selectedRow);
	}
}

UInt8 childCount(TreeNodePtr node) {
	UInt8 count = 0;
	UInt8 i;

	for (i=0; i<NODE_CHILD_CAPACITY; ++i) {
		if (node->children[i] == NULL) {
			return count;
		}
		++count;
	}
	return 0;
}

static void handleMenuItemNode(TreeNodePtr node) {
	StackItem item;

	// Get the menu node inside the choice node
	TreeNodePtr menu = node->children[0];
	if (menu != NULL) {

		// Push the selected node.
		item.node = menu;
		item.selectedRow = 0;
		pushDialogStack(&item);
		drawNode(menu, 0);
	}
}

static void handleTalkNode(TreeNodePtr node) {
	StackItem item;
	item.node = node;
	item.selectedRow = 0;
	menuItemCount = 0;
	messageIndex = 0;
	pushDialogStack(&item);
	hideCursor();
	drawNode(node, 0);
}

static void advanceToNextMessage(void) {
	StackItem item;
	topOfStack(&item);

	++messageIndex;
	if (messageIndex < childCount(item.node)) {
		drawNode(item.node, 0);
	} else {
		exitCurrentNode();
	}
}

static void handleBuyItemNode(TreeNodePtr /* node */) {
	// Show confirmation dialog
}

static SInt8 handleMenuClick(UInt8 index) {
	StackItem item;
	topOfStack(&item);

	if (item.node->value == TalkNode) {
		// Advance to next message or exit talk node.
		advanceToNextMessage();
	} else if (index < childCount(item.node)) {
		TreeNodePtr choice = item.node->children[index];

		// Save selected row of current node.
		popDialogStack(&item);
		item.selectedRow = index;
		pushDialogStack(&item);

		switch (choice->value) {
			case ExitNode:
				exitCurrentNode();
				break;
			case MenuItemNode:
				handleMenuItemNode(choice);
				break;
			case TalkNode:
				handleTalkNode(choice);
				break;
			case BuyItemNode:
				handleBuyItemNode(choice);
				break;
			default: 
				break;
		}
	}
	return isExittingDialog ? MessageReturnToMap : MessageNone;
}

void initDialog(void) {
	UInt16 startTime = SHORT_CLOCK;

	isExittingDialog = 0;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	zeroOut16(TEXT_WINDOW, 7*SCREEN_ROW_BYTES);
	zeroOut16(GRAPHICS_WINDOW, graphicsHeight*SCREEN_ROW_BYTES);
	setPlayerCursorVisible(0);

	// Turn on screen
	loadColorTable(temShopColorTable);
	setScreenMode(ScreenModeDialog);

	// Set up sprites
	clearSprite(3);
	clearSprite(4);
	drawSprite(&temFaceSpriteL, 3, PM_TOP_MARGIN + 22);
	drawSprite(&temFaceSpriteR, 4, PM_TOP_MARGIN + 22);
	setSpriteOriginX(3, PM_LEFT_MARGIN + 72);
	setSpriteOriginX(4, PM_LEFT_MARGIN + 80);
	setSpriteWidth(3, 1);
	setSpriteWidth(4, 1);

	// Set up menu
	initMenu();
	menuIsHorizontal = 0;
	menuItemSpacing = 4;
	setMenuCursor(SmallHeartCursor);
	setMenuClickHandler(handleMenuClick);

	// Set up dialog tree
	{
		StackItem item;
		item.node = &temShopRootNode;
		item.selectedRow = 0;
		pushDialogStack(&item);
		drawNode(item.node, 0);
	}

	// Draw background image
	drawImage(&temShopImage, 0, graphicsHeight);
	debugPrint("Init:", SHORT_CLOCK - startTime, 0, 6);
}

