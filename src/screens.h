#ifndef SCREENS_H
#define SCREENS_H

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

//----------------------------------------------------------------------------------
// Global Variables Declaration (shared by several modules)
//----------------------------------------------------------------------------------
extern GameScreen currentScreen;

//Store all your CD Files Here
//the number is how many files
//you eventually want to load.
u_long* cdData[16];

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

static void ChangeToScreen(int screen);

//----------------------------------------------------------------------------------
// Logo Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitLogoScreen(void);
void StartLogoScreen(void);
void UpdateLogoScreen(void);
void DrawLogoScreen(GsOT *ot);
void UnloadLogoScreen(void);
void FinishLogoScreen(void);

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitTitleScreen(void);
void StartTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(GsOT *ot);
void UnloadTitleScreen(void);
void FinishTitleScreen(void);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitGameplayScreen(void);
void StartGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(GsOT *ot);
void UnloadGameplayScreen(void);
void FinishGameplayScreen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitEndingScreen(void);
void StartEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(GsOT *ot);
void UnloadEndingScreen(void);
void FinishEndingScreen(void);

#ifdef __cplusplus
}
#endif

#endif