#ifndef __WINDOW_H__
#define __WINDOW_H__

#ifdef WIN32
#include <windows.h>
#else
#include <GL/glx.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

class window {
public:
	window();
	void init_SDL();
	void apply_sdl_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip);
	void resize();
	void close_font();
	void quit(int code);

	const SDL_VideoInfo* info;
	SDL_Surface* message;
	SDL_Surface* screen;
	TTF_Font* font;
	int window_w;
	int window_h;

private:
	int window_bpp;
	int window_flags;
	bool isFullScreen;
};

#endif
