#define Sprite GsSPRITE
#define ROT_ONE 4096
#define TYPE_LINE 0
#define TYPE_BOX 1

#define	SPRITE_NORMAL					0
#define SPRITE_BRIGHTNESS				(1 << 6)
#define SPRITE_SEMI_TRANS				(1 << 30)
#define SPRITE_TRANS_50_PLUS_50			(SPRITE_SEMI_TRANS)
#define SPRITE_TRANS_100_PLUS_100		(SPRITE_SEMI_TRANS | (1 << 28))
#define SPRITE_TRANS_100_MINUS_100		(SPRITE_SEMI_TRANS | (2 << 28))
#define SPRITE_TRANS_100_PLUS_25		(SPRITE_SEMI_TRANS | (3 << 28))
#define SPRITE_NO_DISPLAY				(1 << 31)

#define	SPRITE_ATTRIB_MODE_MASK			0x03000000

#define	MAX_CELL	(32 * 16)

#define	ImageToVRamSize(size, mode)			((size) / (1 << (2 - ((mode) & 3))))
#define	VRamToImageSize(size, mode)			((size) * (1 << (2 - ((mode) & 3))))

#define CHARACTERS_FONT_ROW 			32

enum
{
	Tim_4BITS,
	Tim_8BITS,
	Tim_16BITS,
	Tim_24BITS
};

typedef struct {
	GsLINE line;
} Line;

typedef struct {
	Line line[4];
	short width, height;
} Box;

typedef struct {
	RECT rect;
	RECT crect;
	GsIMAGE tim_data;
	GsSPRITE sprite;
} Image;

typedef	struct
{
	GsBG		bg;
	GsMAP		map;
	GsCELL		cell[MAX_CELL];
}	Background;

u_short arr[80] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
			11, 12, 13, 14, 15, 16, 17, 18, 19,
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
			31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
			41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
			51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
			61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
			71, 72, 73, 74, 75, 76, 77, 78, 79};
u_short *pArr = arr;

void InitBackground(Background* p, GsIMAGE* image, u_long attrib, u_short mapWidth, u_short mapHeight, u_short cellSize);
void DrawTextAlign(GsIMAGE *image, char *text, short x, short y, short fontSize, short space, short textBoxWidth, u_short align, Color color, GsOT* ot);


void LoadTIM(GsIMAGE *image, u_long *adrs)
{
	RECT rect;

	GsGetTimInfo(adrs + 1, image);

	rect.x = image->px;
	rect.y = image->py;
	rect.w = image->pw;
	rect.h = image->ph;
	LoadImage(&rect, image->pixel);

	if ((image->pmode & 3) < Tim_16BITS)
	{
		rect.x = image->cx;
		rect.y = image->cy;
		rect.w = image->cw;
		rect.h = image->ch;
		LoadImage(&rect, image->clut);
	}
}

// InitSprite(GsSPRITE Pointer, GsIMAGE Pointer, Sprite Attribute, U Coordinate, V Coordinate, Width, Height);
void InitSprite(GsSPRITE *spr, GsIMAGE *ig, u_long attrib, u_short ix, u_short iy, u_short xs, u_short ys)
{
	// Note: The image can be anywhere in the VRAM (not necessarily at 0,0 of a tpage, and can be larger than 256 pixels, this function take care of this for 4/8/16 bits mode sprites)
	u_short	vrx = (ig->px + ImageToVRamSize(ix, ig->pmode)) & 0x3C0;
	u_short	vry = (ig->py + iy) & 0x100;

	spr->attribute = attrib | ((ig->pmode & 3) << 24);
	spr->w = xs;
	spr->h = ys;
	spr->tpage = getTPage(ig->pmode, 0, vrx, vry);
	spr->u = (VRamToImageSize(ig->px - vrx, ig->pmode) + ix) & 255;
	spr->v = (ig->py + iy) & 255;
	spr->cx = ig->cx;
	spr->cy = ig->cy;
	spr->r = spr->g = spr->b = 128;
	spr->mx = 0;
	spr->my = 0;
	spr->scalex = ONE;
	spr->scaley = ONE;
	spr->rotate = 0;
}

void InitBackground(Background* p, GsIMAGE* image, u_long attrib, u_short mapWidth, u_short mapHeight, u_short cellSize)
{
	GsBG *bg = &p->bg;
	GsMAP *mp = &p->map;
	
	int cellNumberX, cellNumberTotal, i;
	
	cellNumberX = VRamToImageSize(image->pw, image->pmode) / cellSize;
	cellNumberTotal = cellNumberX * (image->ph / cellSize);
	
	bg->attribute = ((image->pmode & 3) << 24) | attrib;
	
	if (mapWidth * cellSize >= 320)
	{
		bg->x = -320/2;
		bg->w = 320;
	}
	else	// If Map is smaller than screen, then center the map !
	{
		bg->w = mapWidth * cellSize;
		bg->x = (320 - bg->w) / 2;
	}
	
	if ((mapHeight * cellSize) >= 240)
	{
		bg->y = -240/2;
		bg->h = 240;
	}
	else	// If Map is smaller than screen, then center the map !
	{
		bg->h = mapHeight * cellSize;
		bg->y = (240 - bg->h) / 2;
	}
	
	bg->scrollx = 0;
	bg->scrolly = 0;
	bg->r = bg->g = bg->b = 128;
	bg->mx = bg->my = 0;
	bg->scalex = bg->scaley = ONE;
	bg->rotate = 0;
	bg->map = mp;

	mp->cellw = mp->cellh = cellSize;
	mp->ncellw = mapWidth;
	mp->ncellh = mapHeight;
	mp->base = p->cell;
	mp->index = pArr;

	for (i = 0; i < cellNumberTotal; i++)
	{
		GsCELL	*cl = &p->cell[i];
		u_short	u, v;

		u = ((i % cellNumberX) * cellSize);
		v = (i / cellNumberX) * cellSize;
		cl->u = u & 255;
		cl->v = v & 255;
		cl->cba = getClut(image->cx, image->cy);
		cl->flag = 0;
		cl->tpage = GetTPage(image->pmode & 3, 0, image->px, image->py) + ((u >> 8) * (1 << (image->pmode & 3)));	// In case our tileset go over the texture page (256px) !
	}
}

void CreateLine(Color* color, int x1, int y1, int x2, int y2, Line* line) {
	line->line.attribute = 0;
	line->line.r = color->r;
	line->line.g = color->g;
	line->line.b = color->b;

	line->line.x0 = x1;
	line->line.y0 = y1;
	line->line.x1 = x2;
	line->line.y1 = y2;
}

void CreateBox(Box* box, int x1, int y1, int x2, int y2, Color *color) {
	box->width = x2;
	box->height = y2;
	
	CreateLine(color, x1, y1, x1 + x2, y1, &box -> line[0]);
	CreateLine(color, x1, y1 + y2, x1 + x2, y1 + y2, &box -> line[1]);
	CreateLine(color, x1, y1, x1, y1 + y2, &box -> line[2]);
	CreateLine(color, x1 + x2, y1, x1 + x2, y1 + y2, &box -> line[3]);
}

void MoveSprite(Sprite* sprite, int x, int y) {
	sprite->x = x;
	sprite->y = y;
}

void RotateSprite(Sprite* sprite, int rotation) {
	sprite -> rotate = ROT_ONE * rotation;
}

void SetOriginSprite(Sprite* sprite, int relative_x, int relative_y) {
	sprite->mx = relative_x;
	sprite->my = relative_y;
}

void ScaleSprite(Sprite* sprite, int x, int y)
{
	sprite->scalex = ONE * x;
	sprite->scaley = ONE * y;
}

// ChangeSprite(GsSPRITE Pointer, GsIMAGE Pointer, U Coordinate, V Coordinate);
void ChangeSprite(GsSPRITE *spr, GsIMAGE *ig, u_short ix, u_short iy)
{
	u_short	vrx = (ig->px + ImageToVRamSize(ix, ig->pmode)) & 0x3C0;
	u_short	vry = (ig->py + iy) & 0x100;

	spr->tpage = getTPage(ig->pmode, 0, vrx, vry);
	spr->u = (VRamToImageSize(ig->px - vrx, ig->pmode) + ix) & 255;
	spr->v = (ig->py + iy) & 255;
}

void BlendColorSprite(Sprite* sprite, Color* color) {
	sprite -> r = color->r;
	sprite -> g = color->g;
	sprite -> b = color->b;
}

void BlendRGBSprite(Sprite* sprite, int r, int g, int b) {
	sprite -> r = r;
	sprite -> g = g;
	sprite -> b = b;
}

void FadeSprite(GsSPRITE *sprite, int value){
	sprite->r = sprite->g = sprite->b = value;
}

void FadeColor(Color *color, int value)
{
	color->r = color->g = color->b = value;
}

// Sprite_AutoFadeIn(GsSPRITE Pointer, Fade Speed);
void AutoFadeInSprite(GsSPRITE *spr, int fspeed)
{
	if (spr->b < 0x80)
		spr->r = spr->g = spr->b += fspeed;
	else
		spr->r = spr->g = spr->b = 0x80;
}


// Sprite_AutoFadeOut(GsSPRITE Pointer, Fade Speed);
void AutoFadeOutSprite(GsSPRITE *spr, int fspeed)
{
	if (spr->b > 0)
		spr->r = spr->g = spr->b -= fspeed;
	else
		spr->r = spr->g = spr->b = 0;
}

void MoveLine(Line* line, int x1, int y1, int x2, int y2) {
	line->line.x0 = x1;
	line->line.y0 = y1;
	line->line.x1 = x2;
	line->line.y1 = y2;
}

void MoveBox(Box* box, int x1, int y1) {
	// Move the lines of the box
	MoveLine(&box->line[0], x1, y1, x1 + box->width, y1);
	MoveLine(&box->line[1], x1, y1 + box->height, x1 + box->width, y1 + box->height);
	MoveLine(&box->line[2], x1, y1, x1, y1 + box->height);
	MoveLine(&box->line[3], x1 + box->width, y1, x1 + box->width, y1 + box->height);
}


void DrawLine(Line *line, GsOT* ot) {
	GsSortLine(&line->line, ot, Priority--);
}

void DrawBox(Box *box, GsOT* ot) {
	int i;
	for(i = 0; i < 4; i++) {
		DrawLine(&box->line[i], ot);
	}
}

void DrawSprite(Sprite *sprite, GsOT *ot) {
	GsSortSprite(sprite, ot, Priority--);
}

void DrawSpriteUI(Sprite *sprite, GsOT *ot){
	GsSortSprite(sprite, ot, 0);
}

void DrawBackground(Background *bg, GsOT *ot) {
	GsSortFastBg(&bg->bg, ot, Priority--);
}

void DrawRectangle(GsBOXF *box, GsOT *ot)
{
	GsSortBoxFill(box, ot, Priority--);
}

void DrawRectangleUI(GsBOXF *box, GsOT *ot)
{
	GsSortBoxFill(box, ot, 0);
}

// DrawText(Font Image, Text Position X/Y, 
//Text, Font Letter Width/Height, Font Letter Real Width, Convert Min to Maj, Text Zone Width (0 if none), Center Text in ZoneW);
void DrawText(GsIMAGE *image, char *text, short x, short y, short fontSize, short space, Color color, GsOT* ot)
{
	DrawTextAlign(image, text, x, y, fontSize, space, 0, 1, color, ot);
}

void DrawTextExp(GsIMAGE *image, char *text, short x, short y, short fontSize, short space, short textBoxWidth, Color color, GsOT* ot)
{
	DrawTextAlign(image, text, x, y, fontSize, space, textBoxWidth, 1, color, ot);
}

void DrawTextAlign(GsIMAGE *image, char *text, short x, short y, short fontSize, short space, short textBoxWidth, u_short align, Color color, GsOT* ot)
{
	GsSPRITE letter;
	register short lw = VRamToImageSize(image->pw, image->pmode)/CHARACTERS_FONT_ROW;
	register short lh = lw;
	register short nletter = VRamToImageSize(image->pw, image->pmode) / lw;
	register short zone = 0, xs;
	register short size = fontSize / lw;
	register short rlw = (lw * size) + space;


	InitSprite(&letter, image, SPRITE_NORMAL, 0, 0, lw, lh);
	
	if (align == 1)
		x += (textBoxWidth - (strlen(text) * rlw)) / 2;

	xs = x;
	while (*text)
	{
		char	c = *text++;

		// Text Warp + Line Feed
		if (((textBoxWidth > 0) && ((zone + rlw) > textBoxWidth)) || (c == '\n'))
		{
			zone = 0;
			x = xs;
			y += rlw;

			if (c == '\n')
				continue;
		}

		// Draw Letter (if not blank space)
		c -= ' ';
		if (c)
		{
			c--;
			ChangeSprite(&letter, image, (c % nletter) * lw, (c / nletter) * lh);
			MoveSprite(&letter, x, y);
			ScaleSprite(&letter, size, size);
			BlendColorSprite(&letter, &color);
			DrawSpriteUI(&letter, ot);
		}

		// Next Letter
		x += rlw;
		zone += rlw;
	}
}