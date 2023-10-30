#include "screens.h"
#include "atlas.h"
#include "TextBox.c"
#include "Gameplay.h"
#include "Inventory.c"
#include "Level1.c"
#include "Level2.c"


//How fast the car drives
int movementSpeed = 5;

//how fast the car turns
int rotationSpeed = 30;

u_short framesCounter = 0;
u_short currentModel = 0;

// Every TMD model you load needs a struct
//as showd below

struct {
	VECTOR position;
	SVECTOR rotation;
} grid;

GameState gameState = GAME;
GameLevel gameLevel = LEVEL1;


void Controls (void);
void PlayGuyWalk (void);


void InitGameplayScreen(void)
{
	
    // TODO: Initialize GAMEPLAY screen variables here!
	ReadCDInit();

	//do all the necessary steps for getting
	//the screen ready to display things
	initializeScreen();

	//get the controller system ready for
	//reading input from the controller
	initializePad();

    //Start reading the CD
	cd_open();

	//read a specific file by name and
	//store it in the cdData variable.
	//(make sure to edit mkpsxiso/cuesheet.xml and
    //add it there or it won't be included on the CD)
    //The number is the slot you want to store the file into.
	cd_read_file("CHICA.TMD", &cdData[0]);
	cd_read_file("CHICA.TIM", &cdData[1]);
	//cd_read_file("TEX02.TIM", &cdData[5]);
	//cd_read_file("TEX03.TIM", &cdData[6]);
	cd_read_file("GRID.TMD", &cdData[2]);
	cd_read_file("ATLAS.TIM", &cdData[3]);
	cd_read_file("DIALOGUE.TIM", &cdData[4]);
	cd_read_file("WALK01.VDF", &cdData[7]);

	//Stop reading the CD
	cd_close();

	//load the TIM texture into VRAM
	//The number is the slot you want to load from.
	//when we used the cd_read_file function, we
	//stored the CAR.TIM texture in slot 1

	blackImage.x = -320/2;
	blackImage.y = -256/2;
	blackImage.w = 320;
	blackImage.h = 256;
	blackImage.r = blackImage.g = blackImage.b = 0;
	blackImage.attribute = SPRITE_TRANS_100_MINUS_100;

    LoadTexture((u_char *)cdData[1]);
    //LoadTexture((u_char *)cdData[5]);
    //LoadTexture((u_char *)cdData[6]);
	
	LoadTIM(&uiImage, cdData[3]);
	LoadTIM(&fontImage, cdData[4]);
	
	InitSprite(&key.item.itemSprite, &uiImage, 0, rtpDescAtlas[1].positionX, 
	rtpDescAtlas[1].positionY, rtpDescAtlas[1].sourceWidth, rtpDescAtlas[1].sourceHeight);
	
	InitSprite(&medallionRight.item.itemSprite, &uiImage, 0, rtpDescAtlas[0].positionX, 
	rtpDescAtlas[0].positionY, rtpDescAtlas[0].sourceWidth, rtpDescAtlas[0].sourceHeight);
	
	InitSprite(&medallion.itemSprite, &uiImage, 0, rtpDescAtlas[2].positionX, 
	rtpDescAtlas[2].positionY, rtpDescAtlas[2].sourceWidth, rtpDescAtlas[2].sourceHeight);
	
	InitSprite(&medallionLeft.itemSprite, &uiImage, 0, rtpDescAtlas[3].positionX, 
	rtpDescAtlas[3].positionY, rtpDescAtlas[3].sourceWidth, rtpDescAtlas[3].sourceHeight);
	
	medallion.itemId = 5;
	medallion.itemIdToCombine = 0;
	medallion.itemDescription = "Medallon completo";
	
	combinedItems[0] = &medallion;
	
	//void InitBackground(Background* p, GsIMAGE* image, u_long attrib, u_short mapWidth, u_short mapHeight, u_short cellSize)
	//InitBackground(&currentRoomBackground, &room01Image, 0, 10, 8, 32);
	
	InitLevel1();

	currentRoom = 1;
	
	InitTextBox(-320/2, 256/6, 319, 256/3, BLACK, &fontImage);

	Sound_CD_Play(2, 2);

}

void StartGameplayScreen(void)
{
	// Load TMD models

	//ObjectCount is an int defined in 3D.c
	//Every time we load a model, we increment this number.
	//Sometimes a TMD model will have more than one model
	//in the file so the LoadTMD function will return the
	//number of objects it found in whatever TMD file you loaded.

	//The LoadTMD function loads a TMD model, and stores
	//it in the Object variable defined in 3D.c

	//LoadTMD(
    //    the TMD Model to load,
    //    The Object variable to store the TMD model in,
    //    Lighting on=1 off=0
    //);
	ObjectCount += LoadTMD(cdData[0], &Object[0], 1); /* Base Model */
	ObjectCount += LoadTMD(cdData[2], &Object[2], 1); /* Base Model */
	
	// Load MIME data
	// Model index, model animation, MIME Data, First MIME vertex data
	InitMime(0, cdData[0], cdData[7], cdData[7] - (0x2000));
	
	//Set all the initial starting positions and
	//rotations here for every loaded object

	//the V in vx, vy, and vz basically stands for Vector

	//Camera
	//ChangeRoom(&initRoom);

	//Girl
	girl.position.vx = 0;
	girl.position.vy = 0;
	girl.position.vz = 0;

	girl.rotation.vy = 900;
	
	StartLevel1();
	StartInventory();

	/////////////////////////////////////////////
	////    Setup the scene lighting here    ////
	/////////////////////////////////////////////

	//The background color of the scene
	//in RGB (values between 0-255)
	SetBGColor(157, 176, 209);

	//Set the color of ambient light in RGB
	SetAmbientLight(67, 67, 67);

	//The sunlight color in RGB
	SetSunColor(255, 255, 255);

	//Sunlight direction
	SetSunDirection(0, -1, 1);
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!

	//this gets the status of all the controller buttons
    //like if they are pressed or not.
	padUpdate();

	//do stuff when buttons are pressed (more info below)
	switch(gameState)
	{
		case GAME:{
			// block controls when a textbox is shown
			if (transition == 0)
			{
				if (onDialogue == 0)
					Controls();
				
				switch (gameLevel)
				{
					case LEVEL1:
					{
						UpdateLevel1();
						break;
					}
					case LEVEL2:
					{
						UpdateLevel2();
						break;
					}
				}
			}
			else
			{
				UpdateTransition();
			}
			
			break;
		}
		case INVENTORY:{
			UpdateInventory();
			break;
		}
		case PAUSE:{
			break;
		}
	}
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(GsOT *ot)
{
    // TODO: Draw GAMEPLAY screen here!
	// Calculate the camera and viewpoint
	switch(gameState)
	{
		case GAME:{
			DrawBackground(&currentRoomBackground, ot);
	
			CalculateCamera();
			
			//RenderObject(grid.position, grid.rotation, &Object[2], ot);
			
			RenderObject(girl.position, girl.rotation, &Object[0], ot);
			//RenderObject(grid.position, grid.rotation, &Object[0], ot);
			switch (gameLevel)
			{
				case LEVEL1:
				{
					//if (key.picked == 0 && currentRoom == 2)
						//RenderObject(key.worldPosition, key.rotation, &Object[1], ot);
					
					if (currentRoom == 1)	
					{
						//RenderObject(level2Door.worldPosition, key.rotation, &Object[1], ot);
						//RenderObject(boxMedalion.worldPosition, key.rotation, &Object[1], ot);
					}
					break;
				}
				case LEVEL2:
				{
					//if (medallionRight.picked == 0)
						//RenderObject(medallionRight.worldPosition, medallionRight.rotation, &Object[1], ot);
					
					break;
				}
			}
			
			if (transition == 0)
			{
				if (onDialogue == 1)
					DrawTextBox(currentDialogue, ot);
			}
			else 
				DrawRectangleUI(&blackImage, ot);
			//DrawSpriteUI(&catSprite, ot);*/
			break;
		}
		case INVENTORY:{
			DrawInventory(ot);
			break;
		}
		case PAUSE:{
			break;
		}
	}

	
}

//Do stuff when buttons are pressed
void Controls () {

    //used later to store a vector for local forward movement
    VECTOR newMoveVec;

	if (padCheck(Pad1Left)) {
		//Turn Left
		girl.rotation.vy -= rotationSpeed;
	}

	if (padCheck(Pad1Right)) {
		//Turn Right
		girl.rotation.vy += rotationSpeed;
	}

	if (padCheck(Pad1Up)) {
		//Move forwards
		newMoveVec = Translate(girl.rotation, 0, 0, movementSpeed * ONE/500);
		
		girl.position.vx += newMoveVec.vx;
		girl.position.vy += newMoveVec.vy;
		girl.position.vz += newMoveVec.vz;
		

		if (girl.position.vx <= currentBoundaries.x)
		{
			girl.position.vx = currentBoundaries.x;
		}
		else if (girl.position.vx >= currentBoundaries.xIn)
		{
			girl.position.vx = currentBoundaries.xIn;
		}
	
		if (girl.position.vz <= currentBoundaries.z)
		{
			girl.position.vz = currentBoundaries.z;
		}
		else if (girl.position.vz >= currentBoundaries.zIn)
		{
			girl.position.vz = currentBoundaries.zIn;
		}
		
		PlayGuyWalk();
	}

	if (padCheck(Pad1Down)) {
		//Move backwards
		newMoveVec = Translate(girl.rotation, 0, 0, -movementSpeed * ONE/500);
		girl.position.vx += newMoveVec.vx;
		girl.position.vy += newMoveVec.vy;
		girl.position.vz += newMoveVec.vz;
		
		if (girl.position.vx <= currentBoundaries.x)
		{
			girl.position.vx = currentBoundaries.x;
		}
		else if (girl.position.vx >= currentBoundaries.xIn)
		{
			girl.position.vx = currentBoundaries.xIn;
		}
	
		if (girl.position.vz <= currentBoundaries.z)
		{
			girl.position.vz = currentBoundaries.z;
		}
		else if (girl.position.vz >= currentBoundaries.zIn)
		{
			girl.position.vz = currentBoundaries.zIn;
		}
	}
	
	if (padCheckPressed(Pad1Triangle)){
		//clear_vram();
		OpenInventory();
	}
}

void PlayGuyWalk(){
	framesCounter++;
	if (framesCounter > 5)
	{
		currentModel++;
		
		if (currentModel >= 5)
		{
			currentModel = 0;
		}
		
		framesCounter = 0;
	}
	
	FrameFix(0, currentModel, 512, 5);
	VertexMime(0);
}

void UnloadGameplayScreen(){}

int VectorDistance(VECTOR *pos1, VECTOR *pos2)
{
	return abs(pos1->vx - pos2->vx) + abs(pos1->vz - pos2->vz);
}

void SetStateGame()
{
	onDialogue = 0;
}

void InitRoom(Boundaries boundaries, VECTOR cameraPos, VECTOR cameraRot, GsIMAGE* roomImage, RoomObject* room)
{
	room->roomBoundaries = boundaries;
	room->cameraPosition = cameraPos;
	room->cameraRotation = cameraRot;
	
	InitBackground(&room->roomBackground, roomImage, 0, 10, 8, 32);
}

void InitLevel()
{
	switch (gameLevel)
	{
		case LEVEL1:
		{
			InitLevel1();
			break;
		}
		case LEVEL2:
		{
			InitLevel2();
			break;
		}
	}
}

void StartLevel()
{
	switch (gameLevel)
	{
		case LEVEL1:
		{
			StartLevel1();
			break;
		}
		case LEVEL2:
		{
			break;
		}
	}
}

void UnloadLevel()
{
	switch (gameLevel)
	{
		case LEVEL1:
		{
			UnloadLevel1();
			break;
		}
		case LEVEL2:
		{
			UnloadLevel2();
			break;
		}
	}
}

void ChangeRoom(RoomObject* newRoom)
{
	currentBoundaries.x = newRoom->roomBoundaries.x;
	currentBoundaries.xIn = newRoom->roomBoundaries.xIn;
	currentBoundaries.z = newRoom->roomBoundaries.z;
	currentBoundaries.zIn = newRoom->roomBoundaries.zIn;
	
	Camera.position.vx = newRoom->cameraPosition.vx;
	Camera.position.vy = newRoom->cameraPosition.vy;
	Camera.position.vz = newRoom->cameraPosition.vz;

	Camera.rotation.vx = newRoom->cameraRotation.vx;
	Camera.rotation.vy = newRoom->cameraRotation.vy;
	Camera.rotation.vz = newRoom->cameraRotation.vz;
	
	currentRoomBackground = newRoom->roomBackground;
}

void ChangeLevel(int level)
{
	UnloadLevel();
	gameLevel = level;
	
	InitLevel();
	StartLevel();
}

void StartTransition(void (*onTransitionEnded)())
{
	TransitionEnded = onTransitionEnded;
	transition = 1;
}

void UpdateTransition()
{
	
	if (fu == 0)
	{
		blackImage.r = blackImage.g = blackImage.b+=2;
		if (blackImage.r == 128)
		{
			fu++;
			TransitionEnded();
		}
	}
	else
	{
		blackImage.r = blackImage.g = blackImage.b-=2;
		if (blackImage.r == 0)
		{
			transition = 0;
			fu = 0;
		}
	}
}