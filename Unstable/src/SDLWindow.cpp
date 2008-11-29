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

void SDLWindow::initConsole(const char* fontpath) {
//    printf("FONT!|%s|\n",fontpath);
    pconsole.texthandler = new DynText(fontpath,18,1,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f);
	pconsole.isHighlighted = false;
}

void SDLWindow::resize() {
    glViewport(0, 0, window_w, window_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0f, (double)window_w / (double)window_h, 1.0f, 100000.0f);
}

void SDLWindow::startGUI() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,window_w, 0, window_h);
    glScalef(1, -1, 1);
    glTranslatef(0, -window_h, 0);
    glMatrixMode(GL_MODELVIEW);
}


void SDLWindow::endGUI() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void SDLWindow::GLDraw(MainRenderer* renderer) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	renderer->draw();
	pool->getCurrentCamera()->update();

	glPushMatrix();
    startGUI();
    glLoadIdentity();
    pconsole.draw(window_w,window_h);
	endGUI();
	glPopMatrix();

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
        case SDLK_ESCAPE:
            if (type == SDL_KEYDOWN)
                pconsole.escape();
            break;
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
            if (!pconsole.isHighlighted)
				KeyDownTrue(&pool->activePlayer->isMovingDown,type);
            break;
        case SDLK_z:
			if (!pconsole.isHighlighted)
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
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE) pconsole.backSpace();
                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (pconsole.isTextInTypeBuffer()) {
                        pconsole.isHighlighted = false;
//                        ProcessConsoleCommand(pconsole.enter().c_str());
                    }
                }
                if (event.key.keysym.sym > 31 && event.key.keysym.sym < 159) {
                    if (pconsole.isHighlighted == false && event.key.keysym.sym != SDLK_x && event.key.keysym.sym != SDLK_z) {
                        pool->activePlayer->SetStill();
                        pconsole.isHighlighted = true;
                    }
                    if (pconsole.isHighlighted) {
                        pconsole.addCharToTypeBuffer(char(event.key.keysym.unicode));
                    }
                }
            }
            break;
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
