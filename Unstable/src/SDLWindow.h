#ifndef __SDLWINDOW_H__
#define __SDLWINDOW_H__

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

class SDLWindow {
public:
	SDLWindow();
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
