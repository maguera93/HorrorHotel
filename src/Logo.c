#include "screens.h"

GsIMAGE logoImage;
GsIMAGE fontImage;
GsSPRITE logoSprite;

Color textColor;
int fadeValue = 0;
int fadeUp = 0;
int f = 0;


//Init logo variable
void InitLogoScreen(void)
{
	ReadCDInit();
	
	//Start reading the CD
	cd_open();

	cd_read_file("LOGO.TIM", &cdData[0]);
	cd_read_file("DIALOGUE.TIM", &cdData[4]);
	cd_read_file("LOGOS.VAG", &cdData[1]);

	cd_close();
	
	LoadTIM(&logoImage, cdData[0]);
	LoadTIM(&fontImage, cdData[4]);
	InitSprite(&logoSprite, &logoImage, SPRITE_TRANS_100_PLUS_100, 0, 0, 128, 64);
	
	Sound_Load(0, (u_char*)cdData[1]);

	SetOriginSprite(&logoSprite, 32, 32);
	ScaleSprite(&logoSprite, 150/64, 150/64);
	MoveSprite(&logoSprite, 0, -32);
	FadeSprite(&logoSprite, 0);
	
	CdInit();
	Sound_CD_Init();

	Sound_CD_Play(3, 1);
}

//Start logo variables
void StartLogoScreen(void)
{
	textColor = GREEN;
}

//Update logo Screen
void UpdateLogoScreen(void)
{
	
	FadeSprite(&logoSprite, fadeValue);
	FadeColor(&textColor, fadeValue);
	
	if (fadeUp == 0)
	{
		fadeValue+=2;
		if (fadeValue == 128)
		{
			++fadeUp;


		}
	}
	else if (fadeUp == 1)
	{
		if (f == 60*2)
			++fadeUp;
		
		++f;

	}
	else if (fadeUp == 2)
	{
		fadeValue-=2;
		if (fadeValue == 0)
		{
			++fadeUp;
		}
	}
	else{
		ChangeToScreen(GAMEPLAY);
	}
	

	

}

//Draw logo Screen
void DrawLogoScreen(GsOT *ot)
{
	//CalculateCamera();
	SetBGColor(0, 0, 0);
	DrawSprite(&logoSprite, ot);
	DrawTextExp(&fontImage, "RELAJAO\nSTUDIO", 80, 32, 16, 4, 0, textColor, ot);
}

//Unload logo Screen
void UnloadLogoScreen(void)
{
	//Sound_Free(0);
	clear_vram();
}

//Finish Logo Screen
void FinishLogoScreen(void)
{
	
}