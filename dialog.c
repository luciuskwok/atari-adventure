// dialog.c

#include "dialog.h"
#include "cursor.h"
#include "graphics.h"
#include "images.h"
#include "image_data.h"
#include "sprites.h"
#include "text.h"
#include <stdio.h>
#include <string.h>


// Constants
enum DialogSpecialValue {
	MenuNodeTypeA = 0xFFF1,
	MenuNodeTypeB = 0xFFF2,
	BuyNode = 0xFFF3,
	SellNode = 0xFFF4,
	TalkNode = 0xFFF5,
	ExitNode = 0xFFFF
};
#define DIALOG_STACK_CAPACITY (16)
#define NODE_CHILD_CAPACITY (6)


// Type
typedef struct tnode *TreeNodePtr;
typedef struct tnode {
	UInt8 *text;
	UInt16 value;
	struct tnode *children[NODE_CHILD_CAPACITY]; 
		// Message nodes: >1 node: menu choices.
		//     1 node = message continues on next node.
		//     0 nodes = message ends, pop stack.
		// NULL terminates list.
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
TreeNode buyFlake1Node = { "tem flake", BuyNode, { &buyFlake1InfoNode, NULL } };

TreeNode buyFlake2InfoNode = { "Heals 2HP\nfood of tem", 1, { NULL } };
TreeNode buyFlake2Node = { "tem flake (onsale,)", BuyNode, { &buyFlake2InfoNode, NULL } };

TreeNode buyFlake3InfoNode = { "Heals 2HP\nfood of tem\n(expensiv)", 10, { NULL } };
TreeNode buyFlake3Node = { "tem flake (expensiv)", BuyNode, { &buyFlake3InfoNode, NULL } };

TreeNode buyCollegeInfoNode = { "COLLEGE\tem pursu higher education", 1000, { NULL } };
TreeNode buyCollegeNode = { "tem pay for colleg", BuyNode, { &buyCollegeInfoNode, NULL } };

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

TreeNode temShopTalkHelloChoiceNode = { "Say Hello", 0, { NULL } };

TreeNode temShopTalkArmorChoiceNode = { "About Temmie Armor", 0, { NULL } };

TreeNode temShopTalkHistoryChoiceNode = { "Temmie History", 0, { NULL } };

TreeNode temShopTalkShopChoiceNode = { "About Shop", 0, { NULL } };

TreeNode temShopTalkMenuNode = { 
	"HOI!!!\n\nim temmie",
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

TreeNode temShopBuyChoiceNode = { "Buy", 0, { &temShopBuyMenuNode, NULL } };

TreeNode temShopSellChoiceNode = { "Sell", 0, { NULL } };

TreeNode temShopTalkChoiceNode = { "Talk", 0, { &temShopTalkMenuNode, NULL } };

TreeNode temShopRootNode = { 
	"* hOi!\n\n* welcom to...\n\n* da TEM SHOP!!!", 
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
			if (child->value == BuyNode) {
				// Get additional info 
				TreeNodePtr info = child->children[0];
				UInt8 s[32];
				sprintf(s, "$%u - %s", info->value, child->text);
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

void setCurrentNode(TreeNodePtr node) {
	clearTextWindow();

	if (node->value == MenuNodeTypeA || node->value == MenuNodeTypeB) {
		PointU8 pt;
		UInt8 width;

		drawVerticalDivider(28);

		if (node->value == MenuNodeTypeA) {
			pt.x = 4;
			width = 24;
		} else {
			pt.x = 30;
			width = 10;
		}
		pt.y = 1;
		drawTextBox(node->text, &pt, width);
	}

	drawMenu(node);
	drawStatus();
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

void printStackDebuggingInfo(UInt8 x) {
	// Debugging
	UInt8 s[16];
	UInt8 i;

	sprintf(s, "%u", x);
	printString(s, 0, 0);

	for (i=0; i<dialogStackCount; ++i) {
		sprintf(s, "[%u]=%u", i, dialogStack[i].selectedRow);
		printString(s, i*6, 6);		
	}
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
		setCurrentNode(item.node);

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

void clickMenuItem(void) {
	StackItem item;

	// Pop item, update its selected row, and push it back on stack.
	popStack(&item);
	item.selectedRow = selectedRow;
	pushStack(&item);

	// Check if node is valid
	if (selectedRow < childCount(item.node)) {
		TreeNodePtr choiceNode = item.node->children[selectedRow];

		// Check for exit
		if (choiceNode->value == ExitNode) {
			exitCurrentNode();
		} else {
			// Get the menu node inside the choice node
			TreeNodePtr menuNode = choiceNode->children[0];
			if (menuNode != NULL) {
				item.node = menuNode;
				item.selectedRow = 0;
				pushStack(&item);
				setCurrentNode(menuNode);
				setSelectedRow(0);
			}
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
	// setTextWindowColorTheme(1);
	// setBackgroundGradient(gradient);
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
		setCurrentNode(item.node);
	}

	// Draw background image
	// {
	// 	SInt8 err = drawImage(temShopImage, temShopImageLength);
	// 	if (err) {
	// 		UInt8 *s;
	// 		sprintf(s, "puff() error:%c", err);
	// 		printString(s, 1, 1);
	// 		waitForAnyInput();
	// 	}
	// }

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
		clickMenuItem();
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
