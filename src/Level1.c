#include "Gameplay.h"

VECTOR doorRoom1;

RoomObject initRoom;
RoomObject leftRoom;

GsIMAGE room01Image;
GsIMAGE room02Image;

void InitKey(void);
void InitLevel2Door(void);
void InitAmuletLeft(void);
void InitAmuletLeftBox(void);
void AddKey(void);
void AddAmuletLeft(void);
void GoToRoom1(void);
void GoToRoom2(void);
void GoToLevel2(void);
void TransitionToLevel2(void);

void InitLevel1()
{
	//Start reading the CD
	cd_open();
	
	cd_read_file("ROOM01.TIM", &cdData[8]);
	cd_read_file("CUBE.TMD", &cdData[9]);
	cd_read_file("ROOM02.TIM", &cdData[10]);
		
	//Stop reading the CD
	cd_close();
	
	LoadTIM(&room01Image, cdData[8]);
	LoadTIM(&room02Image, cdData[10]);
	
	InitKey();
	InitLevel2Door();
	InitAmuletLeft();
	InitAmuletLeftBox();
		/*Init Room*/
			//-3500 4000
		// -2000 3500
		
	doorRoom1.vx = -5000;
	doorRoom1.vy = 0;
	doorRoom1.vz = 1500;
		
	currentBoundaries.x = -5000; 
	currentBoundaries.xIn = 5000; 
	currentBoundaries.z = -4000; 
	currentBoundaries.zIn = 3500; 
	
	currentCameraPos.vx = 2000;
	currentCameraPos.vy = 3000;
	currentCameraPos.vz = 5000;

	currentCameraRot.vx = 200;
	currentCameraRot.vy = -200;
	currentCameraRot.vz = 0;
	
	InitRoom(currentBoundaries, currentCameraPos, currentCameraRot, &room01Image, &initRoom);
	
	/*Left Room*/
			//-4000 3500
		// -5000 1000

	currentBoundaries.x = -2000; 
	currentBoundaries.xIn = 3500; 
	currentBoundaries.z = -5000; 
	currentBoundaries.zIn = 1000; 
	
	currentCameraPos.vx = 2000;
	currentCameraPos.vy = 4000;
	currentCameraPos.vz = 5000;

	currentCameraRot.vx = 400;
	currentCameraRot.vy = -500;
	currentCameraRot.vz = 0;
	
	InitRoom(currentBoundaries, currentCameraPos, currentCameraRot, &room02Image, &leftRoom);
	
	ChangeRoom(&initRoom);
	
	gameLevel = LEVEL1;
	
	CdInit();
	Sound_CD_Init();
}

void StartLevel1()
{	
	ObjectCount += LoadTMD(cdData[9], &Object[1], 1); /* Cube PH */
}

void UpdateLevel1()
{
	if (onDialogue == 1)
	{
		UpdateTextBox(currentDialogue);
		return;
	}
	
	if (currentRoom == 2){
		if (padCheckPressed(Pad1Cross) && VectorDistance(&girl.position, &key.worldPosition) <= 1000)
		{
			if (key.picked == 0)
			{
				onDialogue = 1;
				currentDialogue = &key.dialogue;
			}
		}
		
		if (VectorDistance(&girl.position, &doorRoom1) <= 1000 && entranceExit == 1)
		{
			StartTransition(&GoToRoom1);
		}
		else if (VectorDistance(&girl.position, &doorRoom1) > 1000 && entranceExit == 0)
		{
			entranceExit = 1;
		}
	}
	else
	{	
		if (VectorDistance(&girl.position, &doorRoom1) <= 1000 && entranceExit == 1)
		{
			StartTransition(&GoToRoom2);
		}
		else if (VectorDistance(&girl.position, &doorRoom1) > 1000 && entranceExit == 0)
		{
			entranceExit = 1;
		}
		
		if (VectorDistance(&girl.position, &level2Door.worldPosition) <= 1000 && level2Door.exit == 1)
		{
			level2Door.exit = 0;
			
			if (level2Door.opened == 0)
			{
				onDialogue = 1;
				
				if (CheckHasItem(level2Door.itemId) == 1)
					currentDialogue = &level2Door.dialogue;
				else
					currentDialogue = &level2Door.dialogueNoObject;
			}
			else
			{
				TransitionToLevel2();
			}
		}
		else if(VectorDistance(&girl.position, &level2Door.worldPosition) > 1000 && level2Door.exit == 0)
		{
			level2Door.exit = 1;
		}
		
		if (VectorDistance(&girl.position, &boxMedalion.worldPosition) <= 1000 && boxMedalion.exit == 1)
		{
			boxMedalion.exit = 0;
			
			if (boxMedalion.opened == 0)
			{
				onDialogue = 1;
				
				if (CheckHasItem(boxMedalion.itemId) == 1)
					currentDialogue = &boxMedalion.dialogue;
				else
					currentDialogue = &boxMedalion.dialogueNoObject;
			}
		}
		else if(VectorDistance(&girl.position, &boxMedalion.worldPosition) > 1000 && boxMedalion.exit == 0)
		{
			boxMedalion.exit = 1;
		}
	}
}

void UnloadLevel1()
{	
	free(&cdData[8]);
	free(&cdData[9]);
	free(&cdData[10]);
	
	free(&room01Image);
	free(&room02Image);
	
	free(&initRoom);
	free(&leftRoom);
	//clear_vram();
}

void InitKey(){
	
	if (key.initialized == 1)
		return;
	
	key.worldPosition.vx = 3000;
	key.worldPosition.vy = 0;
	key.worldPosition.vz = 0;
	
	key.item.itemId = 1;
	key.item.itemIdToCombine = 0;
	key.item.itemDescription = "Esta llave me la encontre";
	
	key.dialogue.text[0] = "Hay una llave en el suelo";
	key.dialogue.text[1] = "Quieres cogerla?";
	key.dialogue.dialogueType = QUESTION;
	key.dialogue.lastPage = 2;
	key.dialogue.onAccept = &AddKey;
	key.dialogue.onDecline = &SetStateGame;
	
	key.picked = 0;
	key.initialized = 1;
}

void InitLevel2Door()
{
	if (level2Door.initialized == 1)
		return;
	
	level2Door.worldPosition.vx = 3000;
	level2Door.worldPosition.vy = 0;
	level2Door.worldPosition.vz = 0;
	
	level2Door.dialogueNoObject.text[0] = "La puerta esta cerrada";
	level2Door.dialogueNoObject.dialogueType = TEXT;
	level2Door.dialogueNoObject.onDecline = &SetStateGame;
	
	level2Door.dialogue.text[0] = "La llave encaja en la puerta";
	level2Door.dialogue.text[1] = "Quieres usar la llave?";
	level2Door.dialogue.dialogueType = QUESTION;
	level2Door.dialogue.lastPage = 2;
	level2Door.dialogue.onAccept = &TransitionToLevel2;
	level2Door.dialogue.onDecline = &SetStateGame;
	
	level2Door.itemId = 1;
	level2Door.opened = 0;
	
	level2Door.exit = 0;
	
	level2Door.initialized = 1;
}

void InitAmuletLeftBox()
{
	if (boxMedalion.initialized == 1)
		return;
	
	boxMedalion.worldPosition.vx = -3000;
	boxMedalion.worldPosition.vy = 0;
	boxMedalion.worldPosition.vz = 4000;
	
	boxMedalion.dialogueNoObject.text[0] = "La puerta esta cerrada";
	boxMedalion.dialogueNoObject.dialogueType = TEXT;
	boxMedalion.dialogueNoObject.onDecline = &SetStateGame;
	
	boxMedalion.dialogue.text[0] = "Puedo usar el clip para abrir \nla caja";
	boxMedalion.dialogue.text[1] = "Usar el clip?";
	boxMedalion.dialogue.dialogueType = QUESTION;
	boxMedalion.dialogue.lastPage = 2;
	boxMedalion.dialogue.onAccept = &AddAmuletLeft;
	boxMedalion.dialogue.onDecline = &SetStateGame;
	
	boxMedalion.itemId = 1;
	boxMedalion.opened = 0;
	
	boxMedalion.exit = 0;
	
	boxMedalion.used = 0;
	
	boxMedalion.initialized = 1;
}

void InitAmuletLeft()
{
	medallionLeft.itemId = 2;
	medallionLeft.itemIdToCombine = 3;
	medallionLeft.itemDescription = "Parte izquierda de un medallon";
	
	/*
	medallionLeft.dialogue.text[0] = "Hay una llave en el suelo";
	medallionLeft.dialogue.text[1] = "Quieres cogerla?";
	medallionLeft.dialogue.dialogueType = QUESTION;
	medallionLeft.dialogue.lastPage = 2;
	medallionLeft.dialogue.onAccept = &AddKey;
	medallionLeft.dialogue.onDecline = &SetStateGame;
	
	medallionLeft.picked = 0;
	medallionLeft.initialized = 0;*/
}

void AddKey()
{
	key.picked = 1;
	AddItem(&key.item);	
	SetStateGame();
}

void AddAmuletLeft()
{
	boxMedalion.used = 1;
	AddItem(&medallionLeft);	
	SetStateGame();
}

void GoToRoom1()
{
	girl.position.vx = -5000;
	girl.position.vy = 0;
	girl.position.vz = 1500;
	
	doorRoom1.vx = -5000;
	doorRoom1.vy = 0;
	doorRoom1.vz = 1500;
	
	entranceExit = 0;

	currentRoom = 1;

	ChangeRoom(&initRoom);
}

void GoToRoom2()
{
	girl.position.vx = 0;
	girl.position.vy = 0;
	girl.position.vz = -4500;
	
	doorRoom1.vx = 0;
	doorRoom1.vy = 0;
	doorRoom1.vz = -4500;
	
	entranceExit = 0;

	currentRoom = 2;

	ChangeRoom(&leftRoom);
}

void GoToLevel2()
{
	level2Door.opened = 1;
	//RemoveItem(level2Door.itemId);
	ChangeLevel(LEVEL2);
	SetStateGame();
}

void TransitionToLevel2()
{
	StartTransition(&GoToLevel2);
}