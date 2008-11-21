#ifndef __SDLWINDOW_H__
#define __SDLWINDOW_H__

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include "Draw/MainRenderer.h"

class SDLWindow {
public:
	DataPool* pool;
	void KeyCallback(SDL_keysym* keysym,unsigned int type);
	void KeyDownTrue(bool* var, unsigned press_type);
	SDLWindow(DataPool* pool);
	void ProcessEvents();
	void GLDraw(MainRenderer* renderer);
	void GL_init();
	void init_SDL();
	void resize();
	void quit(int code);

	const SDL_VideoInfo* info;
	SDL_Surface* message;
	SDL_Surface* screen;
	int window_w;
	int window_h;

private:
	int window_bpp;
	int window_flags;
	bool isFullScreen;
};

#endif
