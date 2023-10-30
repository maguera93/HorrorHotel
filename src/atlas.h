//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// rTexpacker v2.5 Atlas Descriptor Code exporter v2.0                          //
//                                                                              //
// more info and bugs-report:  github.com/raylibtech/rtools                     //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2020-2022 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define ATLAS_ATLAS_IMAGE_PATH      "atlas.png"
#define ATLAS_ATLAS_SPRITE_COUNT    4

// Atlas sprite properties
typedef struct rtpAtlasSprite {
    const char *nameId;
    u_short originX, originY;
    u_short positionX, positionY;
    u_short sourceWidth, sourceHeight;
    u_short padding;
    bool trimmed;
    u_short trimRecX, trimRecY, trimRecWidth, trimRecHeight;
} rtpAtlasSprite;

// Atlas sprites array
static rtpAtlasSprite rtpDescAtlas[4] = {
    { "medalionRight", 0, 0, 0, 0, 32, 32, 0, false, 0, 0, 32, 32 },
    { "clip", 0, 0, 32, 0, 32, 32, 0, false, 0, 0, 32, 32 },
    { "medalion", 0, 0, 64, 0, 32, 32, 0, false, 0, 0, 32, 32 },
    { "medalionLeft", 0, 0, 96, 0, 32, 32, 0, false, 0, 0, 32, 32 },
};
