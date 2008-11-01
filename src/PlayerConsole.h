#ifndef __PLAYERCONSOLE_H__
#define __PLAYERCONSOLE_H__

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>

#include <SDL/SDL_opengl.h>

#include "DynText.h"


class PlayerConsole {
public:
	void draw();
	void addLineToMainBuffer(std::string line);
	std::string enter();
	void addCharToTypeBuffer(char c);
	std::string TypeBuffer;
	DynText * texthandler;
private:
	std::vector<std::string> scrolltxt;
};

#endif
