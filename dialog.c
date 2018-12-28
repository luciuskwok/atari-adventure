// dialog.c

#include "dialog.h"
#include "cursor.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
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

UInt8 isExittingDialog;

PointU8 menuOrigin;
UInt8 menuItemCount;
UInt8 selectedRow;


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


// Testing strings:
UInt8 sansMessage[] = "Sans: Why are graveyards so noisy?\n Because of all the *coffin*!";
UInt8 ellieMessage[] = "Ellie: How are you doing today? That teacher was totally unfair. C'mon, let's go to the beach!";
UInt8 papyrusMessage[] = "Papyrus: Nyeh Heh Heh!";


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

	// Update globals
	menuOrigin.x = x;
	menuOrigin.y = y;
	menuItemCount = 0;

	for (i=0; i<6; ++i) {
		child = node->children[i];
		if (child) {
			if (child->value == BuyItemNode) {
				// Get additional info 
				TreeNodePtr info = child->children[0];
				UInt8 s[40] = "$";
				numberString(s+1, 0, info->value);
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
	// Draw money and potion count.
	printString("$901", 28, 6);
	printString("21{", 37, 6);
}

void drawNode(TreeNodePtr node) {
	PointU8 pt = { 4, 1 };
	UInt8 lineSpacing;
	UInt8 width;

	clearTextWindow();

	if (node->value == MenuNodeTypeA || node->value == MenuNodeTypeB) {
		drawVerticalDivider(28);

		if (node->value == MenuNodeTypeA) {
			pt.x = 4;
			width = 24;
			lineSpacing = 2;
		} else {
			pt.x = 30;
			width = 10;
			lineSpacing = 1;
		}
		drawTextBox(node->text, &pt, width, lineSpacing);
		drawMenu(node);
		drawStatus();
	} else if (node->value == TalkNode) {
		// Use selectedRow as the index of the message to show.
		TreeNodePtr message = node->children[selectedRow];
		if (message) {
			pt.x = 4;
			width = 32;
			lineSpacing = 2;
			drawTextBox(message->text, &pt, width, lineSpacing);
		}
	}
}

void setCursorPosition(PointU8 *newPos) {
	const UInt8 topMargin = 14;
	static UInt8 previousY = 0;

	// Remove old sprite data
	drawSprite(NULL, selectionCursorSpriteHeight, 1, topMargin + 4 * previousY);
	
	// Draw sprite in new position
	drawSprite(selectionCursorSprite, selectionCursorSpriteHeight, 1, topMargin + 4 * newPos->y);
	setSpriteHorizontalPosition(1, PM_LEFT_MARGIN - 8 + 4 * newPos->x);

	previousY = newPos->y;
}

void hideCursor(void) {
	setSpriteHorizontalPosition(1, 0);
}

void setSelectedRow(UInt8 index) {
	const UInt8 topMargin = 18;
	PointU8 cell;
	cell.x = menuOrigin.x;
	cell.y = menuOrigin.y + index + topMargin;
	setCursorPosition(&cell);
	selectedRow = index;
}

void exitCurrentNode(void) {
	popStack(NULL);

	if (isStackEmpty()) {
		isExittingDialog = 1;
	} else {
		StackItem item;
		topOfStack(&item);

		// Draw previous node
		drawNode(item.node);

		// Restore selection row
		setSelectedRow(item.selectedRow);
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

void handleMenuItemNode(TreeNodePtr node) {
	StackItem item;

	// Get the menu node inside the choice node
	TreeNodePtr menu = node->children[0];
	if (menu != NULL) {

		// Push the selected node.
		item.node = menu;
		item.selectedRow = 0;
		pushStack(&item);
		drawNode(menu);
		setSelectedRow(0);
	}
}

void handleTalkNode(TreeNodePtr node) {
	StackItem item;
	item.node = node;
	item.selectedRow = 0;
	selectedRow = 0;
	pushStack(&item);
	hideCursor();
	drawNode(node);
}

void advanceToNextMessage(void) {
	StackItem item;
	topOfStack(&item);

	++selectedRow;
	if (selectedRow < childCount(item.node)) {
		drawNode(item.node);
	} else {
		exitCurrentNode();
	}
}

void handleBuyItemNode(TreeNodePtr node) {
	// Show confirmation dialog
}

void handleClick(void) {
	StackItem item;
	topOfStack(&item);

	if (item.node->value == TalkNode) {
		// Advance to next message or exit talk node.
		advanceToNextMessage();
	} else if (selectedRow < childCount(item.node)) {
		TreeNodePtr choice = item.node->children[selectedRow];

		// Save selected row of current node.
		popStack(&item);
		item.selectedRow = selectedRow;
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
}

void initDialog(void) {
	// Set up graphics window
	fadeOutColorTable(FadeTextBox);
	setScreenMode(ScreenModeOff);
	clearTextWindow();
	clearRasterScreen();
	setPlayerCursorVisible(0);

	// Turn on screen
	loadColorTable(temShopColorTable);
	setScreenMode(ScreenModeShop);

	// Set up sprites
	clearSpriteData(3);
	clearSpriteData(4);
	drawSprite(temFaceSprite, temFaceSpriteHeight, 3, 22+14);
	drawSprite(temFaceSprite+temFaceSpriteHeight, temFaceSpriteHeight, 4, 22+14);
	setSpriteHorizontalPosition(3, PM_LEFT_MARGIN + 72);
	setSpriteHorizontalPosition(4, PM_LEFT_MARGIN + 80);

	// Set up dialog tree
	{
		StackItem item;
		item.node = &temShopRootNode;
		item.selectedRow = 0;
		pushStack(&item);
		drawNode(item.node);
	}

	// Draw background image
	{
		// UInt16 duration;
		// UInt8 s[16] = "Time: ";
		// UInt16 startTime = SHORT_CLOCK;
		SInt8 err = drawImage(temShopImage, temShopImageLength);
		if (err) {
			UInt8 message[20] = "puff() error:";
			numberString(message+13, 0, err);
			printString(message, 1, 0);
		}
		// duration = SHORT_CLOCK - startTime;
		// numberString(s+6, 0, duration);
		// printString(s, 1, 2);
	}

	// Selection Cursor
	clearSpriteData(1);
	setPlayerCursorColorCycling(1);
	setSelectedRow(0);

	registerCursorEventHandler(dialogCursorHandler);
	isExittingDialog = 0;
}

void exitDialog(void) {
	// Fade out
	fadeOutColorTable(FadeGradient | FadeTextBox);
	clearSpriteData(4);
	hideSprites();
}

SInt8 dialogCursorHandler(UInt8 event) {

	if (event == CursorClick) {
		handleClick();
		if (isExittingDialog != 0) {
			return MessageExitDialog;
		} 
	} else {
		UInt8 newRow = selectedRow;
		switch (event) {
			case CursorUp:
				--newRow;
				break;
			case CursorDown:
				++newRow;
				break;
			case CursorLeft:
				break;
			case CursorRight:
				break;
		}
		if (newRow != selectedRow && newRow < menuItemCount) {
			setSelectedRow(newRow);
		}
	}
	return MessageNone;
}
