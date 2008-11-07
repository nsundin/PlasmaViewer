#include "SDLWindow.h"

#define defaultScreenX 800
#define defaultScreenY 600

SDLWindow::SDLWindow() {
	info = NULL;
	message = NULL;
	screen = NULL;
}

void SDLWindow::GL_init() {
    glDisable(GL_LIGHTING);
    glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
}

void SDLWindow::init_SDL() {
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

    SDL_WM_SetCaption("PlasmaViewer", "PlasmaViewer");
    SDL_EnableUNICODE(1);

    window_flags = SDL_OPENGL | SDL_RESIZABLE; //SDL_NOFRAME like linkpanel O.O
    window_bpp = info->vfmt->BitsPerPixel;

	screen = SDL_SetVideoMode(window_w, window_h, window_bpp, window_flags);

    if(screen == NULL) {
        printf("[FAIL] SDL_SetVideoMode: %s\n", SDL_GetError());
        quit(1);  
    }

	resize();
}

void SDLWindow::resize() {
    glViewport(0, 0, window_w, window_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0f, (double)window_w / (double)window_h, 1.0f, 100000.0f);
}

void SDLWindow::quit(int code) {
    SDL_Quit();
    exit(code);
}
