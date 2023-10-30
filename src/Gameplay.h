#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#define MAX_ITEM_SLOTS 5
#define MAX_ITEM_COMBINED 8

typedef enum GameState { GAME, INVENTORY, PAUSE } GameState;

typedef enum GameLevel { LEVEL1, LEVEL2 } GameLevel;

struct {
	VECTOR position;
	SVECTOR rotation;
} girl;

typedef struct Item{
	GsSPRITE itemSprite;
	u_short itemId;
	u_short itemIdToCombine;
	u_char* itemDescription;
	u_short uiPosX, uiPosY;
} Item;

typedef struct DialogueObject{
	VECTOR worldPosition;
	Dialogue dialogue;
} DialogueObject;

typedef struct ItemObject{
	VECTOR worldPosition;
	SVECTOR rotation;
	Item item;
	Dialogue dialogue;
	u_short picked;
	u_short initialized;
} ItemObject;
typedef struct DoorObject{
	VECTOR worldPosition;
	Dialogue dialogue;
	Dialogue dialogueNoObject;
	u_short itemId;
	u_short opened;
	u_short initialized;
	u_short exit;
	u_short used;
} DoorObject;

typedef struct Boundaries{
	int x;
	int z;
	int xIn;
	int zIn;
}Boundaries;

typedef struct RoomObject{
	Boundaries roomBoundaries;
	VECTOR cameraPosition;
	VECTOR cameraRotation;
	Background roomBackground;
}RoomObject;


u_short entranceExit = 1;
u_short onDialogue = 0;
u_short currentRoom = 0;
u_short fu = 0;

ItemObject key;
Item medallion;
Item medallionLeft;
ItemObject medallionRight;
DoorObject boxMedalion;
DoorObject level2Door;

GsBOXF blackImage;

VECTOR currentCameraPos;
VECTOR currentCameraRot;

Boundaries currentBoundaries;

GsIMAGE uiImage;
GsIMAGE fontImage;

Background currentRoomBackground;

extern GameState gameState;
extern GameLevel gameLevel;

void InitLevel();
void StartLevel();
void UnloadLevel();

int VectorDistance(VECTOR *pos1, VECTOR *pos2);

void SetStateGame();
void (*TransitionEnded)(void);

void InitRoom(Boundaries boundaries, VECTOR cameraPos, VECTOR cameraRot, GsIMAGE* roomImage, RoomObject* room);
void ChangeRoom(RoomObject* newRoom);

void ChangeLevel(int level);

void StartInventory();
void OpenInventory();
void UpdateInventory();
void DrawInventory(GsOT* ot);

void StartTransition(void (*onTransitionEnded)());
void UpdateTransition();
void DrawTransition(GsOT* ot);

void AddItem(Item *newItem);

void NextSlot();
void PrevSlot();

u_short transition;
u_short currentRoom;

#endif