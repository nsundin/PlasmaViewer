#include "window.h"

#define defaultScreenX 800
#define defaultScreenY 600

window::window() {
	info = NULL;
	message = NULL;
	screen = NULL;
	font = NULL;
}

void window::init_SDL() {
    window_w = defaultScreenX;
    window_h = defaultScreenY;

    if(SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        printf("[FAIL] SDL_INIT_EVERYTHING: %s\n", SDL_GetError());
        quit(1);
    }
    info = SDL_GetVideoInfo();
    if(!info) {
        printf("[FAIL] SDL_GetVideoInfo: %s\n",SDL_GetError());
        quit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_WM_SetCaption("PlasmaViewer", "PlasmaViewer");
    SDL_EnableUNICODE(1);

    window_flags = SDL_OPENGL | SDL_SWSURFACE | SDL_RESIZABLE; //SDL_NOFRAME like linkpanel O.O
    window_bpp = info->vfmt->BitsPerPixel;

	screen = SDL_SetVideoMode(window_w, window_h, window_bpp, window_flags);

	resize();

    if(screen == NULL) {
        printf("[FAIL] SDL_SetVideoMode: %s\n", SDL_GetError());
        quit(1);  
    }
    if(TTF_Init() == -1) {
        printf("[FAIL] TTF_Init: %s\n", SDL_GetError());
        quit(1);
    }

	font = TTF_OpenFont( "FreeMono.ttf", 28 );
}

void window::apply_sdl_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL) {
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface(source, clip, destination, &offset);
}

void window::resize() {
    glViewport(0, 0, window_w, window_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0f, (double)window_w / (double)window_h, 1.0f, 100000.0f);
//cam.camFOV
	//    cam.update();
}

void window::close_font() {
    SDL_FreeSurface(message);
    TTF_CloseFont(font);
    TTF_Quit();
}

void window::quit(int code) {
	close_font();
    SDL_Quit();
    exit(code);
}
