#include <STDLIB.H>
#include <STDIO.H>
#include <STDDEF.H>
#include <LIBMATH.H>
#include <LIBGTE.H>
#include <LIBGPU.H>
#include <LIBGS.H>
#include <LIBETC.H>
#include <LIBSPU.H>
#include <LIBDS.H>
#include <LIBCD.H>
#include <libapi.h>	//Kernal Library
#include <STRINGS.H>
#include <SYS/TYPES.H>

//Declare Stuff Here
void clear_vram();

// Screen resolution and dither mode
int SCREEN_WIDTH, SCREEN_HEIGHT;

#define CLITERAL(type)      (type)

#define LIGHTGRAY  CLITERAL(Color){ 200, 200, 200 }   // Light Gray
#define GRAY       CLITERAL(Color){ 130, 130, 130 }   // Gray
#define DARKGRAY   CLITERAL(Color){ 80, 80, 80 }      // Dark Gray
#define YELLOW     CLITERAL(Color){ 253, 249, 0 }     // Yellow
#define GOLD       CLITERAL(Color){ 255, 203, 0 }     // Gold
#define ORANGE     CLITERAL(Color){ 255, 161, 0 }     // Orange
#define PINK       CLITERAL(Color){ 255, 109, 194 }   // Pink
#define RED        CLITERAL(Color){ 230, 41, 55 }     // Red
#define MAROON     CLITERAL(Color){ 190, 33, 55 }     // Maroon
#define GREEN      CLITERAL(Color){ 0, 228, 48 }      // Green
#define LIME       CLITERAL(Color){ 0, 158, 47 }      // Lime
#define DARKGREEN  CLITERAL(Color){ 0, 117, 44 }      // Dark Green
#define SKYBLUE    CLITERAL(Color){ 102, 191, 255 }   // Sky Blue
#define BLUE       CLITERAL(Color){ 0, 121, 241 }     // Blue
#define DARKBLUE   CLITERAL(Color){ 0, 82, 172 }      // Dark Blue
#define PURPLE     CLITERAL(Color){ 200, 122, 255 }   // Purple
#define VIOLET     CLITERAL(Color){ 135, 60, 190 }    // Violet
#define DARKPURPLE CLITERAL(Color){ 112, 31, 126 }    // Dark Purple
#define BEIGE      CLITERAL(Color){ 211, 176, 131 }   // Beige
#define BROWN      CLITERAL(Color){ 127, 106, 79 }    // Brown
#define DARKBROWN  CLITERAL(Color){ 76, 63, 47 }      // Dark Brown

#define WHITE      CLITERAL(Color){ 255, 255, 255 }   // White
#define BLACK      CLITERAL(Color){ 0, 0, 0 }         // Black
#define RAYWHITE   CLITERAL(Color){ 245, 245, 245 }

#define CENTERX	SCREEN_WIDTH/2
#define CENTERY	SCREEN_HEIGHT/2
#define DITHER 1

// Increasing this value (max is 14) reduces sorting errors in certain cases
#define OT_LENGTH	12
#define OT_ENTRIES	1<<OT_LENGTH
#define PACKETMAX	2048

typedef struct Color{
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Color;

// Camera coordinates
struct {
	VECTOR	position;
	SVECTOR rotation;
	GsCOORDINATE2 coord2;
} Camera;

typedef enum { false, true } bool;
 
int	Priority;	// Current Priority (used for each Sort in OT function, so the drawing can be from back to front and not the inverse as it is by default)
GsOT *CurOT;

GsOT		orderingTable[2];
GsOT_TAG	orderingTable_TAG[2][OT_ENTRIES];
int			myActiveBuff=0;
PACKET GPUOutputPacket[2][PACKETMAX*24];
Color BGColor;

//Creates a color from RGB
Color createColor(int r, int g, int b) {
	Color color = {.r = r, .g = g, .b = b};
	return color;
}

void SetBGColor (int r, int g, int  b) {
	BGColor = createColor(r, g, b);
}

void initializeScreen() {

	ResetGraph(0);
	//clear_vram();

	// Automatically adjust screen to PAL or NTCS from license
	if (*(char *)0xbfc7ff52=='E') { // SCEE string address
    	// PAL MODE
    	SCREEN_WIDTH = 320;
    	SCREEN_HEIGHT = 256;
    	printf("Setting the PlayStation Video Mode to (PAL %dx%d)\n",SCREEN_WIDTH,SCREEN_HEIGHT);
    	SetVideoMode(1);
    	printf("Video Mode is (%ld)\n",GetVideoMode());
   	} else {
     	// NTSC MODE
     	SCREEN_WIDTH = 320;
     	SCREEN_HEIGHT = 240;
     	printf("Setting the PlayStation Video Mode to (NTSC %dx%d)\n",SCREEN_WIDTH,SCREEN_HEIGHT);
     	SetVideoMode(0);
     	printf("Video Mode is (%ld)\n",GetVideoMode());
   }
	GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER|GsOFSGPU, 1, 0);
	GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);

	// Prepare the ordering tables
	orderingTable[0].length	=OT_LENGTH;
	orderingTable[1].length	=OT_LENGTH;
	orderingTable[0].org		=orderingTable_TAG[0];
	orderingTable[1].org		=orderingTable_TAG[1];

	GsClearOt(0, 0, &orderingTable[0]);
	GsClearOt(0, 0, &orderingTable[1]);

	// Initialize debug font stream
	FntLoad(960, 256);
	//FntOpen(-CENTERX + 7, -CENTERY + 15, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 512);

	// Setup 3D and projection matrix
	GsInit3D();
	GsSetProjection(CENTERX);
	GsInitCoordinate2(WORLD, &Camera.coord2);

	// Set default lighting mode
	//0 = No Fog
	//1 = Fog
	GsSetLightMode(0);

}

void clear_vram() {
    RECT rectTL;
    setRECT(&rectTL, 0, 0, 1024, 512);
    ClearImage2(&rectTL, 0, 0, 0);
    DrawSync(0);
    return;
}

void clear_display() {

	// Get active buffer ID and clear the OT to be processed for the next frame
	myActiveBuff = GsGetActiveBuff();
	GsSetWorkBase((PACKET*)GPUOutputPacket[myActiveBuff]);
	GsClearOt(0, 0, &orderingTable[myActiveBuff]);

}

GsOT *InitFrame()
{
	myActiveBuff = GsGetActiveBuff();
	
	CurOT = &orderingTable[myActiveBuff];
	GsSetWorkBase(GPUOutputPacket[myActiveBuff]);
	GsClearOt(0, 0, CurOT);

	Priority = (1 << 10) - 1;

	return (CurOT);
}

void Display(GsOT *ot) {

	FntFlush(-1);

	DrawSync(0);
	VSync(0);
	GsSwapDispBuff();
	GsSortClear(BGColor.r, BGColor.g, BGColor.b, &orderingTable[myActiveBuff]);
	GsDrawOt(&orderingTable[myActiveBuff]);

}