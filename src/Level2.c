#include "Gameplay.h"

VECTOR doorRoom1;

RoomObject lvl2Room;

GsIMAGE lvl2Image;

void InitAmuletRight(void);
void AddAmuletRight(void);
void GoToLevel1(void);
void TransitionToLevel1(void);


void InitLevel2()
{
	//Start reading the CD
	cd_open();
	
	cd_read_file("ROOM03.TIM", &cdData[8]);
		
	//Stop reading the CD
	cd_close();
	
	InitAmuletRight();
	LoadTIM(&lvl2Image, cdData[8]);

		/*Init Room*/
			//-3500 4000
		// -2000 3500
		
	doorRoom1.vx = -900;
	doorRoom1.vy = 0;
	doorRoom1.vz = 1200;
	
	entranceExit = 0;

	girl.position.vx = -900;
	girl.position.vy = 0;
	girl.position.vz = 1200;
		
	currentBoundaries.x = -900; 
	currentBoundaries.xIn = 2500; 
	currentBoundaries.z = -3000; 
	currentBoundaries.zIn = 2500; 
	
	currentCameraPos.vx = 1000;
	currentCameraPos.vy = 3000;
	currentCameraPos.vz = 5000;

	currentCameraRot.vx = 250;
	currentCameraRot.vy = -600;
	currentCameraRot.vz = 0;
	
	InitRoom(currentBoundaries, currentCameraPos, currentCameraRot, &lvl2Image, &lvl2Room);
	
	ChangeRoom(&lvl2Room);
	

	
	CdInit();
	Sound_CD_Init();

	Sound_CD_Play(2, 2);

}

void UpdateLevel2()
{
	if (onDialogue == 1)
	{
		UpdateTextBox(currentDialogue);
	}
	
	
	if (VectorDistance(&girl.position, &doorRoom1) <= 1000 && entranceExit == 1)
	{
		TransitionToLevel1();
	}
	else if (VectorDistance(&girl.position, &doorRoom1) > 1000 && entranceExit == 0)
	{
		entranceExit = 1;
	}
	
	if (padCheckPressed(Pad1Cross) && VectorDistance(&girl.position, &medallionRight.worldPosition) <= 1000)
	{
		if (medallionRight.picked == 0)
		{
			onDialogue = 1;
			currentDialogue = &medallionRight.dialogue;
		}
	}
}

void UnloadLevel2()
{	
	free(&cdData[8]);
	
	free(&lvl2Image);
	
	free(&lvl2Room);
	//clear_vram();
}

void InitAmuletRight(){
	
	if (medallionRight.initialized == 1)
		return;
	
	medallionRight.worldPosition.vx = 3000;
	medallionRight.worldPosition.vy = 0;
	medallionRight.worldPosition.vz = 0;
	
	medallionRight.item.itemId = 3;
	medallionRight.item.itemIdToCombine = 2;
	medallionRight.item.itemDescription = "Parte derecha del medellon";
	
	medallionRight.dialogue.text[0] = "Es la parte derecha";
	medallionRight.dialogue.text[1] = "Quieres cogerlo?";
	medallionRight.dialogue.dialogueType = QUESTION;
	medallionRight.dialogue.lastPage = 2;
	medallionRight.dialogue.onAccept = &AddAmuletRight;
	medallionRight.dialogue.onDecline = &SetStateGame;
	
	medallionRight.picked = 0;
	medallionRight.initialized = 1;
}

void AddAmuletRight(){
	medallionRight.picked = 1;
	AddItem(&medallionRight .item);	
	SetStateGame();
}

void GoToLevel1()
{
	girl.position.vx = 5000;
	girl.position.vy = 0;
	girl.position.vz = 1500;
	
	ChangeLevel(LEVEL1);
	SetStateGame();
}

void TransitionToLevel1()
{
	StartTransition(&GoToLevel1);
}