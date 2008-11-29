#ifndef __PLAYERCONSOLE_H__
#define __PLAYERCONSOLE_H__

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

#include <SDL/SDL_opengl.h>

#include "Draw/DynText.h"


class PlayerConsole {
public:
	void escape();
	void CursorMoveRight();
	void CursorMoveLeft();
	bool isHighlighted;
	bool isTextInTypeBuffer();
	void draw(int window_w,int window_h);
	void addLineToMainBuffer(std::string line);
	std::string enter();
	void backSpace();
	void addCharToTypeBuffer(char c);
	DynText * texthandler;
private:
	int cursorInd;
	std::string TypeBuffer;
	std::string ReturnBuffer;
	std::vector<std::string> scrolltxt;
};

#endif