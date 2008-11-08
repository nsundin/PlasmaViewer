#ifndef __MAINENGINE_H__
#define __MAINENGINE_H__

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include "ResManager/plResManager.h"
#include "ResManager/plAgeInfo.h"
#include "Debug/hsExceptions.h"
#include "Debug/plDebug.h"
#include "PlasmaDefs.h"
#include "prpengine.h"
#include "SDLWindow.h"
#include "player.h"
#include "PlayerConsole.h"
#include "DynText.h"
#include "LinkManager.h"

class MainEngine {
public:
	void Init();
	void GL_init();
	void KeyDownTrue(bool* var, unsigned press_type);
	void KeyCallback(SDL_keysym* keysym,unsigned int type);
	void ProcessConsoleCommand(const char * text);
	void ProcessEvents();
	void MotionHandler();
	void startGUI(int window_w,int window_h);
	void endGUI();
	void draw();
	int main(int argc, char* argv[]);

	PlayerConsole plasmaconsole;
	SDLWindow* window;
	plResManager rm;
	camera cam;
	Player currentPlayer;
	prpengine prp_engine;
	LinkManager lnkmgr;
};

#endif
