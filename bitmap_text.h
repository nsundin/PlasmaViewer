#ifndef __BITMAP_TEXT_H__
#define __BITMAP_TEXT_H__

#include <gl/glut.h>

class ScreenTextMgr {
public:
	void printToScreen(char *string);
	void Render(int w, int h);
	char* text;
};
#endif
