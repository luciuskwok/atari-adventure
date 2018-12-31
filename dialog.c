// dialog.c

#include "dialog.h"
#include "cursor.h"
#include "game_chara.h"
#include "graphics.h"
#include "menu.h"
#include "sprites.h"
#include "text.h"
#include "atari_memmap.h"


// Constants
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
	0x58, 0x58, 0x00, 0x00, // sprite
	0x26, 0x0E, 0x00, 0xFF, 0x2A, // playfield
	0x0E, 0x00, 0x82 // text box
};

const DataBlock temShopImage = {
	1150, // length
	{
		0x8D,0xD6,0x31,0x6B,0x23,0x47,0x14,0x00,0xE0,0xD9,0xAC,0xB6,0x31,0xF8,0x56,0x02,
		0xED,0x9F,0x70,0x95,0xDA,0xE0,0x68,0x16,0xAC,0x3A,0x23,0xD8,0xB7,0x42,0x22,0xB1,
		0x2A,0x1F,0x9C,0x2B,0x43,0x0A,0x81,0x73,0x89,0x0E,0x52,0xA7,0xB4,0x83,0x53,0x5D,
		0x63,0x13,0x8F,0x7C,0x38,0x8D,0x49,0x65,0xBC,0xF9,0x01,0x21,0x69,0x96,0xAB,0x36,
		0x6E,0x83,0x52,0x9C,0x8A,0x08,0x81,0x91,0xB2,0x79,0xF3,0x66,0x66,0x77,0xB5,0x76,
		0xE0,0xA6,0x90,0xAD,0xD5,0xA7,0x37,0xF3,0xDE,0xCC,0x3C,0x9B,0x01,0x03,0xF6,0x31,
		0x23,0xFA,0x38,0xE7,0xA0,0xE3,0x1E,0xE0,0x88,0xD4,0x0B,0x8C,0xFE,0xA1,0xC7,0x9D,
		0xE4,0x89,0x73,0x80,0x07,0x50,0x8C,0x83,0x9B,0x3F,0xF0,0xA9,0x3B,0x7B,0xCE,0x89,
		0xAA,0x93,0x77,0xC0,0xD9,0x67,0x32,0x71,0x70,0x06,0xC6,0x8A,0x4F,0xC8,0x79,0x82,
		0xA9,0x47,0xEA,0x7B,0x2F,0xE4,0x34,0x05,0x98,0xC9,0xD4,0x7C,0xDE,0x35,0x3F,0x43,
		0xE1,0xF5,0x44,0xA0,0x9C,0xCE,0x67,0x57,0xCA,0x05,0xF4,0x7F,0xDA,0xB1,0xAE,0xB5,
		0xE1,0x78,0xB1,0x8E,0xDD,0xB7,0x3B,0xD7,0x70,0xF0,0x76,0x8B,0x5C,0x88,0xCE,0x2C,
		0x0F,0x5D,0xC8,0x83,0x72,0xBD,0xBB,0x6F,0x3E,0x99,0xE2,0xCB,0x16,0xE5,0xC1,0x43,
		0x11,0xB2,0x88,0x3E,0xF5,0x78,0x50,0x73,0x0C,0x1D,0x23,0x17,0xF0,0x96,0x00,0x1E,
		0x85,0xD6,0x61,0xAD,0xAB,0x6E,0x27,0xF9,0x4B,0x3B,0x81,0x2E,0xE0,0xD0,0x50,0xCF,
		0x03,0x16,0x38,0x35,0xB7,0x65,0x1D,0x67,0x21,0xA6,0x08,0x42,0x3B,0x4C,0x53,0x6C,
		0xB8,0x4E,0x9E,0x97,0xCE,0xEB,0xE9,0x20,0x58,0x3E,0xE6,0x54,0xDC,0x8B,0x37,0x1D,
		0x4C,0x75,0x55,0x3A,0xB0,0x2E,0xDA,0x74,0xDF,0xA9,0x8A,0xF4,0xFF,0xD6,0x73,0x85,
		0xC2,0x29,0x1C,0xC7,0x5C,0x2A,0xEE,0x7B,0x2A,0xEB,0xD7,0xF4,0x06,0xF3,0x70,0xF4,
		0xF2,0x98,0xF0,0x6A,0xEE,0x16,0xA3,0x01,0x7C,0x61,0x1D,0x33,0x8E,0x23,0xAA,0x6C,
		0x07,0x73,0x53,0x18,0xFD,0x99,0x1A,0xD7,0xC4,0x8D,0x07,0xF3,0x21,0xD6,0x18,0xAA,
		0x0E,0xE0,0xE8,0xF1,0x16,0xFA,0x85,0x0B,0x4A,0x57,0x29,0x9F,0x72,0x52,0x5E,0x60,
		0xC6,0xEA,0x4D,0xA3,0xE2,0x22,0x75,0xA4,0xCB,0xB1,0x0D,0xB1,0x94,0x57,0x7A,0x81,
		0x1E,0x16,0x8D,0x06,0xA3,0x73,0xEF,0x6D,0xBA,0x21,0xB9,0x63,0xFC,0x1D,0x4F,0xA7,
		0x76,0x9C,0x6A,0x17,0x54,0x5C,0xBE,0x56,0x4E,0x02,0x24,0xE4,0x8A,0xF3,0xAC,0x6A,
		0x52,0x49,0xC3,0x97,0xA7,0x70,0xA4,0xDC,0x68,0xAE,0xEE,0x56,0xC8,0x1A,0x34,0xA1,
		0x9E,0xBB,0x70,0x1E,0x9E,0xE6,0x53,0x4C,0x43,0xCA,0xD3,0xE1,0x0D,0x6E,0x4B,0xE4,
		0x95,0x0E,0x67,0x56,0xAF,0x51,0xE0,0xE1,0xE1,0x8D,0x66,0x85,0x93,0x58,0xEF,0xA8,
		0xE1,0x34,0x74,0xA2,0x1E,0x56,0xBB,0x72,0xD9,0x94,0x3B,0x30,0xEE,0x9A,0x05,0xC2,
		0xD3,0x29,0xAA,0xBD,0xC3,0x82,0x97,0xA3,0x8F,0xE2,0x5B,0xE5,0x2E,0x62,0x72,0xB6,
		0x62,0x98,0x29,0x40,0xCD,0x1D,0x18,0x37,0x65,0x41,0x71,0x8C,0xB4,0x33,0xB7,0xC3,
		0x21,0x07,0xE4,0xAE,0x40,0x39,0x9B,0x9F,0xCA,0x02,0xC0,0x56,0x0F,0x2B,0xE9,0x62,
		0x1A,0x27,0xDA,0xC5,0x53,0x15,0x42,0x78,0x7A,0xA2,0x62,0x75,0xC2,0xB8,0x5B,0xEB,
		0x0E,0xA6,0xD5,0x25,0x29,0xD7,0x53,0xE1,0x9B,0xE4,0xD8,0xEC,0x1D,0xB6,0x0C,0x72,
		0x27,0x15,0x27,0x68,0xEE,0x50,0xB5,0x10,0xEB,0xA6,0x29,0xB9,0x29,0x9C,0x5C,0xAB,
		0xC2,0x53,0x2B,0xA1,0x75,0x46,0x9E,0xD3,0x8D,0xA0,0xDB,0xD6,0x6E,0x57,0xDE,0x92,
		0x93,0xF9,0x63,0xDD,0x39,0xE8,0x98,0xD7,0xB0,0xEE,0x5E,0xBB,0x5F,0x4E,0x7E,0xAE,
		0xBB,0x30,0x14,0xCC,0xD9,0xD7,0xCE,0x9F,0x19,0x77,0xF7,0x98,0x50,0xA9,0xA8,0x2C,
		0x9C,0x8E,0x33,0xEF,0x89,0x66,0xD8,0xD5,0x8E,0x75,0xD6,0x26,0xDE,0x07,0x66,0x9D,
		0x53,0x3A,0x6C,0x5B,0xC6,0xF9,0x4B,0x13,0x6F,0xC5,0x2C,0xD1,0x9B,0x47,0xAE,0xC1,
		0x7A,0xC6,0x6D,0x67,0x26,0xDE,0x31,0xB3,0x53,0x5A,0x17,0x0A,0xB1,0xCF,0xB1,0x88,
		0x8E,0xF0,0xF3,0x95,0x3F,0xA6,0x3A,0xCB,0xF5,0xDC,0x5C,0x1E,0xF3,0x7D,0x74,0xDD,
		0x48,0x04,0xD8,0xC2,0xF0,0x7D,0x27,0x5F,0x6D,0x67,0x8F,0xE4,0x96,0xC7,0xC5,0x19,
		0x27,0x87,0x87,0x00,0xDA,0xF8,0x4B,0xDB,0x38,0x7F,0x4C,0xE7,0x6A,0x9A,0xCD,0x59,
		0x75,0x38,0x80,0x7D,0xB0,0x1D,0x59,0x07,0xAF,0xDD,0xEC,0x41,0xB9,0x9B,0xB1,0x2A,
		0x8B,0x4B,0xB5,0xD1,0x27,0x06,0x9D,0x3E,0xD0,0x42,0xBB,0xF1,0x58,0x25,0x72,0x97,
		0x29,0xE2,0xAF,0x88,0xA8,0x3F,0x2F,0x91,0xC7,0xCD,0x46,0x37,0xD1,0xA5,0x18,0x2F,
		0xFB,0x17,0xA7,0x5D,0x52,0xBC,0xBD,0x55,0x00,0x42,0x3B,0x6C,0x43,0x6D,0x7D,0x1C,
		0xD1,0xF9,0xE9,0x04,0xE3,0x4D,0x1E,0x65,0x9E,0x51,0xBC,0xC5,0x04,0x4F,0x13,0xDE,
		0x75,0x75,0x8B,0xAA,0xCE,0x5D,0x4C,0xDC,0x6C,0x9D,0xE5,0xF9,0x72,0x3C,0x56,0xCB,
		0x4B,0x73,0x74,0xB8,0xF9,0x74,0x67,0x6B,0x6E,0xFC,0xFB,0x38,0xCF,0xD7,0xD9,0x43,
		0xE7,0x57,0xF8,0xBC,0xBF,0xA2,0x2A,0x07,0x1B,0x0E,0x17,0xD1,0xC7,0x89,0xB2,0xDF,
		0x28,0xDE,0x72,0x76,0x07,0xDF,0x8C,0x12,0xBA,0x35,0x50,0x77,0xB0,0x98,0xA4,0x0F,
		0xEF,0x73,0x35,0x26,0xF7,0x1F,0x92,0xBD,0x11,0xD3,0x2D,0x03,0xC8,0x79,0xA5,0x5B,
		0x8F,0xD2,0x2F,0xD3,0xAF,0xF2,0x7C,0xB5,0x37,0x92,0xD7,0xC9,0xE2,0x35,0x5D,0x35,
		0x1D,0xB1,0x70,0x4D,0x55,0x88,0xD1,0xDC,0x4F,0x5C,0x74,0x9F,0xBA,0xF2,0x26,0x49,
		0xE7,0xAA,0xE5,0xE1,0x95,0xEC,0x52,0xBE,0x9E,0x28,0x5C,0x67,0x34,0x77,0xC9,0xE1,
		0x4D,0xB9,0x77,0xDF,0x27,0xB6,0x35,0xE2,0x0F,0xED,0xA8,0x1F,0x46,0xCC,0xEF,0xE3,
		0xB6,0xFA,0xB9,0x1D,0xAC,0x68,0x8D,0xE4,0xCC,0x4A,0x59,0x1B,0xC9,0xAA,0x53,0x28,
		0x15,0x75,0xC3,0xF1,0xA6,0x7E,0xD3,0xB0,0xA1,0xD6,0xBA,0xD9,0xCC,0x55,0x72,0xD1,
		0xB3,0x6E,0x99,0x2F,0xCA,0xA6,0x34,0xD7,0x45,0xC0,0x34,0x75,0x1E,0xA5,0x73,0xD7,
		0xD5,0xE6,0x35,0xB7,0xE5,0x03,0x72,0x0E,0x6F,0xE9,0xCE,0x8B,0x77,0xB8,0xEE,0x98,
		0xF9,0x7F,0x83,0x93,0x73,0x78,0xD5,0xF5,0x73,0x9A,0x7C,0xF8,0x83,0x72,0x01,0x37,
		0x1B,0xF2,0x34,0x1E,0xB9,0x41,0x7C,0x6E,0x1C,0x4D,0x8E,0x2F,0xC2,0xB3,0x6D,0xB3,
		0x9C,0x37,0xBE,0x1C,0x0C,0x7E,0x2C,0x1C,0xDD,0x4C,0xE5,0x6C,0x17,0x27,0x77,0x74,
		0x39,0x84,0x61,0x3C,0x78,0x75,0x4E,0xEB,0xF3,0xAC,0x73,0xF0,0x5E,0x18,0x87,0x4F,
		0x72,0x18,0x1E,0xC6,0x12,0x67,0x3D,0x3A,0x34,0x4E,0x3C,0xEF,0xE2,0xAB,0x57,0xF1,
		0x25,0x0C,0xCF,0xCE,0x5F,0x9E,0x95,0xAE,0x25,0xEA,0xAE,0x03,0x87,0xC3,0xB3,0x97,
		0x00,0x67,0xF1,0x51,0xAC,0x5D,0xC4,0x59,0xA3,0xFB,0x9C,0x1B,0xC4,0x31,0xC6,0x3B,
		0x33,0xF5,0x73,0x7A,0x2D,0xA7,0x15,0xFC,0x9F,0x03,0x38,0xB5,0x2E,0xDA,0xDF,0xDF,
		0xF7,0x9E,0x9D,0xF7,0xA2,0xBA,0x1F,0xF8,0xEF,0x61,0x8B,0x8A,0x53,0x77,0x83,0xCD,
		0x7D,0xA3,0xA6,0xF6,0xD4,0xE5,0xF5,0xFD,0xA5,0xCE,0xA6,0xB6,0xEB,0x3F,
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


void pushStack(StackItem *item) {
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

void popStack(StackItem *outItem) {
	topOfStack(outItem);
	if (dialogStackCount > 0) { 
		--dialogStackCount;
	}
}

void popStackRoot(StackItem *outItem) {
	if (dialogStackCount > 1) {
		dialogStackCount = 1;
	}
	popStack(outItem);
}

UInt8 isStackEmpty(void) {
	return dialogStackCount == 0;
}

void drawVerticalDivider(UInt8 x) {
	UInt8 y;
	for (y=0; y<7; ++y) {
		printString("|", x, y);
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
				printString(s, x, y);
			} else {
				printString(child->text, x, y);
			}
			++y;
			++menuItemCount;
		} else {
			break;
		}
	}
}

void drawStatus(void) {
	UInt8 s[8];
	UInt8 len;

	// Draw money and potion count.
	printString("$", 28, 6);
	uint16toString(s, partyMoney);
	printString(s, 29, 6);

	printString("{", 39, 6);
	uint8toString(s, partyPotions);
	len = stringLength(s);
	printString(s, 39-len, 6);
}

void drawNode(TreeNodePtr node, UInt8 selectIndex) {
	const UInt8 y = 1;
	UInt8 lineSpacing;
	UInt8 width;
	UInt8 x;

	clearTextWindow(7);

	if (node->value == MenuNodeTypeA || node->value == MenuNodeTypeB) {
		drawVerticalDivider(28);

		if (node->value == MenuNodeTypeA) {
			x = 4;
			width = 24;
			lineSpacing = 2;
		} else {
			x = 30;
			width = 10;
			lineSpacing = 1;
		}
		drawTextBox(node->text, x, y, width, lineSpacing, 0);
		drawMenu(node);
		setMenuSelectedIndex(selectIndex);
		drawStatus();
	} else if (node->value == TalkNode) {
		TreeNodePtr message = node->children[messageIndex];
		if (message) {
			x = 2;
			width = 32;
			lineSpacing = 2;
			drawTextBox(message->text, x, y, width, lineSpacing, -2);
		}
	}
}

void exitCurrentNode(void) {
	popStack(NULL);

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
		pushStack(&item);
		drawNode(menu, 0);
	}
}

static void handleTalkNode(TreeNodePtr node) {
	StackItem item;
	item.node = node;
	item.selectedRow = 0;
	menuItemCount = 0;
	messageIndex = 0;
	pushStack(&item);
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
		popStack(&item);
		item.selectedRow = index;
		pushStack(&item);

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
	UInt16 duration;
	SInt8 err;

	isExittingDialog = 0;

	// Set up graphics window
	setScreenMode(ScreenModeOff);
	clearTextWindow(7);
	clearRasterScreen(72);
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
		pushStack(&item);
		drawNode(item.node, 0);
	}

	// Draw background image
	err = drawImage(&temShopImage, 0, 72);
	if (err) {
		debugPrint("puff() error:", err, 0, 0);
	} else {
		UInt8 s[6];
		duration = SHORT_CLOCK - startTime;
		uint16toString(s, duration);
		printString(s, 0, 6);		
	}
}

