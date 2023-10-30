#include "Gameplay.h"

#define SLOT_POS_X 112
#define SLOT_POS_Y 64
#define SLOT_POS_SPACE 48


Item* slots[MAX_ITEM_SLOTS] = {NULL_P};
Item* combinedItems[MAX_ITEM_COMBINED] = {NULL_P};
Item* selectedItem;
Item* selectedItemCombine = NULL_P;
short selectedSlot;

GsBOXF inventoryBox;
GsBOXF slotBox[MAX_ITEM_SLOTS];

Box selectBox;

GsIMAGE itemsImage;
Color sColor = RED;

u_short combining = 0;

void ShowItemDescription(GsOT* ot);
void SetupInventorySlots();
void AddItemById(u_short id);
void CombineItems(Item *fItem, Item* sItem);

void StartInventory()
{
	inventoryBox.x = -320/2;
	inventoryBox.y = -256/2;
	inventoryBox.w = 320;
	inventoryBox.h = 256;
	inventoryBox.r = inventoryBox.g = inventoryBox.b = 0;
	//inventoryBox.attribute = 0;
	
	selectedSlot = 0;
	
	CreateBox(&selectBox, -SLOT_POS_X, -SLOT_POS_Y, 32, 32, &sColor);
	
	SetupInventorySlots();
}

void OpenInventory()
{
	gameState = INVENTORY;
	selectedSlot = 0;
	selectedItem = slots[selectedSlot];
}

void UpdateInventory()
{
	if (padCheckPressed(Pad1Triangle)){
		//clear_vram();
		selectedItemCombine = NULL_P;
		gameState = GAME;
	}
	
	if (padCheckPressed(Pad1Circle)){
		//clear_vram();
		selectedItemCombine = NULL_P;
	}
	
	if (combining == 0 && padCheckPressed(Pad1Cross) && slots[selectedSlot] != NULL_P){
		//clear_vram();
		combining = 1;
		selectedItemCombine = slots[selectedSlot];
		NextSlot();
	}
	else if (combining == 1 && padCheckPressed(Pad1Cross) && slots[selectedSlot] != NULL_P && selectedItemCombine != NULL_P)
	{
		CombineItems(slots[selectedSlot], selectedItemCombine);
		selectedItemCombine = NULL_P;
		combining = 0;
	}
	
	if (padCheckPressed(Pad1Left))
		PrevSlot();
	else if (padCheckPressed(Pad1Right))
		NextSlot();
}

void DrawInventory(GsOT* ot){
	u_short i;
	DrawRectangle(&inventoryBox, ot);
	for (i = 0; i < MAX_ITEM_SLOTS; i++)
	{
		DrawRectangle(&slotBox[i], ot);
		
		if (slots[i] != NULL_P)
		{
			DrawSprite(&slots[i]->itemSprite, ot);
		}
	}
	
	DrawBox(&selectBox, ot);
	DrawTextExp(&fontImage, "INVENTORY", 0, -100, 16, 4, 0, WHITE, ot);	
	ShowItemDescription(ot);
}

void SetupInventorySlots()
{
	u_short i = 0;
	Color boxColor = GRAY;
	
	for(i = 0; i < MAX_ITEM_SLOTS; i++)
	{
		slotBox[i].x = -SLOT_POS_X + SLOT_POS_SPACE * i;
		slotBox[i].y = -SLOT_POS_Y;
		slotBox[i].w = slotBox[i].h = 32;
		slotBox[i].r = boxColor.r;
		slotBox[i].g = boxColor.g;
		slotBox[i].b = boxColor.b;
	}
}

void NextSlot()
{
	selectedSlot++;
	
	if (selectedSlot > MAX_ITEM_SLOTS - 1)
		selectedSlot = MAX_ITEM_SLOTS - 1;
	
	if (selectedItemCombine != NULL_P && selectedItemCombine == slots[selectedSlot])
	{
		if (selectedSlot == MAX_ITEM_SLOTS - 1)
			PrevSlot();
		else
			NextSlot();
	}
	
	MoveBox(&selectBox, -SLOT_POS_X + SLOT_POS_SPACE * selectedSlot, -SLOT_POS_Y);
	
	selectedItem = slots[selectedSlot];
}

void PrevSlot()
{
	selectedSlot--;
	if (selectedSlot < 0)
		selectedSlot = 0;
	
	if (selectedItemCombine != NULL_P && selectedItemCombine == slots[selectedSlot])
	{
		if (selectedSlot == 0)
			NextSlot();
		else
			PrevSlot();
	}
	
	MoveBox(&selectBox, -SLOT_POS_X + SLOT_POS_SPACE * selectedSlot, -SLOT_POS_Y);
	
	selectedItem = slots[selectedSlot];
}

void AddItem(Item *newItem)
{
	u_short i = 0;
	for(i = 0; i < MAX_ITEM_SLOTS; i++)
	{
		if (slots[i] == NULL_P)
		{
			slots[i] = newItem;
			MoveSprite(&newItem->itemSprite, 
			-SLOT_POS_X + SLOT_POS_SPACE * i, -SLOT_POS_Y);
			break;
		}
	}
}

void AddItemById(u_short id)
{
	u_short i = 0;
	for(i = 0; i < MAX_ITEM_COMBINED; i++)
	{
		if (combinedItems[i] != NULL_P && combinedItems[i]->itemId == id)
		{
			AddItem(combinedItems[i]);
			break;
		}
	}
}

void RemoveItem(u_short id)
{
	u_short i = 0;
	
	for(i = 0; i < MAX_ITEM_SLOTS; i++)
	{
		if (slots[i] != NULL_P && slots[i]->itemId == id)
		{
			slots[i] = NULL_P;
			break;
		}
	}
}

void CombineItems(Item *fItem, Item* sItem)
{
	if (fItem->itemIdToCombine == sItem->itemId)
	{
		AddItemById(fItem->itemId + sItem->itemId);
		RemoveItem(fItem->itemId);
		RemoveItem(sItem->itemId);
	}
}

void ShowItemDescription(GsOT* ot)
{
	if (selectedItem != NULL_P)
	{
		DrawTextExp(&fontImage, selectedItem->itemDescription, 0, 0, 8, 4, 0, RED, ot);	
	}
}

u_short CheckHasItem(u_short id)
{
	u_short hasItem = 0;
	u_short i = 0;
	
	for(i = 0; i < MAX_ITEM_SLOTS; i++)
	{
		if (slots[i] != NULL_P && slots[i]->itemId == id)
		{
			hasItem = 1;
			break;
		}
	}
	
	return hasItem;
}