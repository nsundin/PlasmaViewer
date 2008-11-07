#include <stdio.h>
#include <stdlib.h>
#include "DataPool/DataPool.h"
#include "Draw/MainRenderer.h"
#include "SDLWindow.h"
#include "ResManager/plResManager.h"
#include "Control/LinkManager.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_mutex.h"

SDLWindow window;
DataPool pool;
MainRenderer renderer;
LinkManager lnkmgr;
Camera* cam = pool.createCamera();
SDL_mutex* mutex;



void ProcessEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        //if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        //    KeyCallback(&event.key.keysym,event.type);
        //    if (event.type == SDL_KEYDOWN) {
        //        if (event.key.keysym.sym == SDLK_BACKSPACE) plasmaconsole.backSpace();
        //        if (event.key.keysym.sym == SDLK_RETURN) {
        //            if (plasmaconsole.isTextInTypeBuffer()) {
        //                plasmaconsole.isHighlighted = false;
        //                ProcessConsoleCommand(plasmaconsole.enter().c_str());
        //            }
        //        }
        //        if (event.key.keysym.sym > 31 && event.key.keysym.sym < 159) {
        //            if (plasmaconsole.isHighlighted == false && event.key.keysym.sym != SDLK_x && event.key.keysym.sym != SDLK_z) {
        //                currentPlayer.SetStill();
        //                plasmaconsole.isHighlighted = true;
        //            }
        //            if (plasmaconsole.isHighlighted) {
        //                plasmaconsole.addCharToTypeBuffer(char(event.key.keysym.unicode));
        //            }
        //        }
        //    }
        //    break;
        //}
        if (event.type == SDL_VIDEORESIZE && event.resize.w > 0 && event.resize.h > 0) {
            window.window_w = event.resize.w;
            window.window_h = event.resize.h;
            window.resize();
            break;
        }
        if (event.type == SDL_QUIT) {
            window.quit(0);
            break;
        }
    }
}


int DrawFunc(void* arg) {
	printf("Draw\n");
	renderer.UpdateList(false);
	while (1) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.draw();

		SDL_mutexP(mutex);
		cam->turn();
		cam->update();
		SDL_mutexV(mutex);
		SDL_GL_SwapBuffers();
	}
	return 0;
}

int UpdatePos(void* arg) {
	while (1) {
		Sleep(10);
		SDL_mutexP(mutex);
		if (cam->angle > 365.0f)
			cam->angle = 0.0f;
		cam->angle += 0.001f;
		SDL_mutexV(mutex);
	}
	return 0;
}

int main(int argc, char** argv) {
	window.init_SDL();
	window.GL_init();

	renderer.pool = &pool;
	lnkmgr.pool = &pool;
	lnkmgr.rm = new plResManager;
	lnkmgr.Load("C:\\Personal_District_psnlMYSTII.prp");

	SDL_Thread* threads[2];
	mutex = SDL_CreateMutex();
	threads[0] = SDL_CreateThread(DrawFunc,NULL);
	threads[1] = SDL_CreateThread(UpdatePos,NULL);
	while (1) {
		ProcessEvents();
	}
	SDL_DestroyMutex(mutex);
	return 1;
}
