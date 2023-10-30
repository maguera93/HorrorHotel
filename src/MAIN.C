// Some of The code in this project comes from a combination of things from
// Wituz's Playstation Development series on youtube
// and Lameguy64's TMD model viewer example.

//Include all the features you want for your project here
//Each of these files contains a set of functions you can
//use in your projects.

#include "Engine/Constants.c"
#include "Engine/Controller.c"
#include "Engine/ReadCD.c"
#include "Engine/Sound.c"
#include "Engine/2D.c"
#include "Engine/3D.c"
#include "Engine/Animations.c"

#include "screens.h"
#include "Logo.c"
#include "Title.c"
#include "Gameplay.c"
#include "Ending.c"

//Declare any function you make here
void Initialize();
void Start();
void Update();
void Render();

GsOT *ot;


//////////////////////////////////////
////    Game variables go here    ////
//////////////////////////////////////
GameScreen currentScreen = 0;



//The main function is the first
//thing that runs in your game
int main() {

    //all the functions here are described below
	
	currentScreen = GAMEPLAY;
	
	initializeScreen();
	Sound_Init();

	Initialize();
	Start();

	//This is the main loop of your
	//game that will run every frame
	while(1) {
		Update();
		Render();
	}

	return 0;
}

//This function runs once right when the game launches
void Initialize() {

    switch(currentScreen)
	{
		case LOGO: InitLogoScreen(); break;
		case TITLE: InitTitleScreen(); break;
		case GAMEPLAY: InitGameplayScreen(); break;
		case ENDING: InitEndingScreen(); break;
	}
}

//This function runs once right after he Initialize function
//and before the Update function. Do game related stuff
//here before the game begins.
void Start() {

	switch(currentScreen)
	{
		case LOGO: StartLogoScreen(); break;
		case TITLE: StartTitleScreen(); break;
		case GAMEPLAY: StartGameplayScreen(); break;
		case ENDING: StartEndingScreen(); break;
	}
}

//This is where the action happens. This function runs every frame.
void Update () 
{
	switch(currentScreen)
	{
		case LOGO: UpdateLogoScreen(); break;
		case TITLE: UpdateTitleScreen(); break;
		case GAMEPLAY: UpdateGameplayScreen(); break;
		case ENDING: UpdateEndingScreen(); break;
	}

}

//This function runs right after the Update
//function and displays everything on screen.
void Render () {

	// Prepare for rendering
	ot = InitFrame();

	switch(currentScreen)
	{
		case LOGO: DrawLogoScreen(ot); break;
		case TITLE: DrawTitleScreen(ot); break;
		case GAMEPLAY: DrawGameplayScreen(ot); break;
		case ENDING: DrawEndingScreen(ot); break;
	}
	
	Display(ot);
}

static void ChangeToScreen(int screen)
{
    // Unload current screen
    switch (currentScreen)
    {
        case LOGO: UnloadLogoScreen(); break;
        case TITLE: UnloadTitleScreen(); break;
        case GAMEPLAY: UnloadGameplayScreen(); break;
        case ENDING: UnloadEndingScreen(); break;
        default: break;
    }

    currentScreen = screen;
	
	Initialize();
	Start();
}