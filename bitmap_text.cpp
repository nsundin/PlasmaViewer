#include "bitmap_text.h"

void ScreenTextMgr::Render(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glScalef(1, -1, 1);
	glTranslatef(0, -h, 0);
	glMatrixMode(GL_MODELVIEW);
//draw text
	//this->text
	char *c;
	glRasterPos2f(100, 50);
	for (c="Something"; *c != '\0'; c++) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
void ScreenTextMgr::printToScreen(char* text) {
	this->text = text;
}
