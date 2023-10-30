#include "TextBox.h"

Color markColor = GRAY;

void InitTextBox(int x, int y, int width, int height, Color color, GsIMAGE *font)
{
	textBox.x = x;
	textBox.y = y;
	textBox.w = width;
	textBox.h = height;
	textBox.r = color.r;
	textBox.g = color.g;
	textBox.b = color.b;
	
	CreateBox(&textBoxMark, x, y, width, height, &markColor);
	
	
	fontTextImage = font;
	
	InitSprite(&acceptSprite, font, 0, 48, 24, 16, 15);
	InitSprite(&declineSprite, font, 0, 32, 24, 16, 15);
	
	MoveSprite(&acceptSprite, -320/4, 256/3);
	MoveSprite(&declineSprite, 320/4, 256/3);
}

void UpdateTextBox(Dialogue *dialogue)
{
	switch(dialogue->dialogueType)
	{
		case TEXT:
		{
			if (padCheckPressed(Pad1Cross))
			{
				dialogue->currentText++;
				
				if (dialogue->currentText >= dialogue->lastPage - 1)
				{
					dialogue->currentText = 0;
					dialogue->onDecline();
				}
			}

			break;
		}
		case QUESTION:
		{
			if (padCheckPressed(Pad1Cross))
			{
				if (dialogue->currentText < dialogue->lastPage-1)
					dialogue->currentText++;
				else
				{
					dialogue->onAccept();
					dialogue->currentText = 0;
				}
			}
			else if padCheckPressed(Pad1Circle)
			{
				if (dialogue->currentText >= dialogue->lastPage-1)
				{
					dialogue->onDecline();
					dialogue->currentText = 0;
				}
			}
			
			break;
		}
	}

	
	

}

void DrawTextBox(Dialogue *dialogue, GsOT *ot)
{
	register u_short ct = dialogue->currentText;
	register u_short lt = dialogue->lastPage;
	
	if (dialogue->text[ct] == NULL)
	{
		return;
	}
	
	DrawRectangle(&textBox, ot);
	DrawBox(&textBoxMark, ot);
	
	DrawTextAlign(&fontImage, dialogue->text[ct], -textBox.w/2 + 10, textBox.y + 10, 8, 2, textBox.w, 0, WHITE, ot);	
	
	if (dialogue->dialogueType == QUESTION && ct == lt - 1)
	{
		DrawSpriteUI(&acceptSprite, ot);
		DrawTextAlign(&fontImage, "Si", acceptSprite.x + 24, acceptSprite.y + 4, 8, 2, 0, 0, WHITE, ot);
		DrawSpriteUI(&declineSprite, ot);
		DrawTextAlign(&fontImage, "No", declineSprite.x + 24, declineSprite.y + 4, 8, 2, 0, 0, WHITE, ot);
	}
}