#include "SDLWindow.h"

#define defaultScreenX 800
#define defaultScreenY 600

SDLWindow::SDLWindow(DataPool* pool) {
	this->pool = pool;
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

void SDLWindow::GLDraw(MainRenderer* renderer) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	renderer->draw();

	pthread_mutex_lock(pool->mutex);
	cam->update();
	pthread_mutex_unlock(pool->mutex);

	SDL_GL_SwapBuffers();
}
void SDLWindow::quit(int code) {
    SDL_Quit();
//	pthread_exit(0);
    exit(code);
}

void SDLWindow::KeyDownTrue(bool* var, unsigned press_type) {
    if (press_type == SDL_KEYDOWN)
        *var = true;
    else
        *var = false;
}

void SDLWindow::KeyCallback(SDL_keysym* keysym,unsigned int type) {
    switch(keysym->sym) {
//        case SDLK_ESCAPE:
//            if (type == SDL_KEYDOWN)
//                plasmaconsole.escape();
//            break;
 //Player Control Keys
        case SDLK_LEFT:
			KeyDownTrue(&pool->activePlayer->isTurningLeft,type);
			break;
        case SDLK_RIGHT:
			KeyDownTrue(&pool->activePlayer->isTurningRight,type);
            break;
        case SDLK_UP:
            KeyDownTrue(&pool->activePlayer->isMovingForward,type);
            break;
        case SDLK_DOWN:
            KeyDownTrue(&pool->activePlayer->isMovingBackward,type);
            break;
        case SDLK_LSHIFT:
            KeyDownTrue(&pool->activePlayer->isRun,type);
            break;
        case SDLK_RSHIFT:
            KeyDownTrue(&pool->activePlayer->isRun,type);
            break;
        case SDLK_x:
			KeyDownTrue(&pool->activePlayer->isMovingDown,type);
            break;
        case SDLK_z:
			KeyDownTrue(&pool->activePlayer->isMovingUp,type);
            break;
        default:
            break;
    }
}

void SDLWindow::ProcessEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            KeyCallback(&event.key.keysym,event.type);
		}
        else if (event.type == SDL_VIDEORESIZE && event.resize.w > 0 && event.resize.h > 0) {
            window_w = event.resize.w;
            window_h = event.resize.h;
            resize();
            break;
        }
        else if (event.type == SDL_QUIT) {
            quit(0);
            break;
        }
    }
}
