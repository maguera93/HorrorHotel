#ifndef TEXTBOX_H
#define TEXTBOX_H

#define MAX_TEXT 3

typedef enum DialogueType {TEXT, QUESTION} DialogueType;

typedef struct Dialogue{
	DialogueType dialogueType;
	char *text[MAX_TEXT];
	u_short lastPage;
	u_short currentText;
	void (*onAccept)();
	void (*onDecline)();
} Dialogue;

Dialogue* currentDialogue;

GsBOXF textBox;
Box textBoxMark;

GsIMAGE *fontTextImage;

GsSPRITE acceptSprite;
GsSPRITE declineSprite;

void InitTextBox(int x, int y, int width, int height, Color color, GsIMAGE *spritesImage);
void UpdateTextBox(Dialogue *dialogue);
void DrawTextBox(Dialogue *dialogue, GsOT *ot);
#endif