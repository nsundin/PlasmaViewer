#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "DataPool/DataPool.h"
#include "Draw/MainRenderer.h"
#include "SDLWindow.h"
#include "ResManager/plResManager.h"
#include "Control/LinkManager.h"

SDLWindow window;
DataPool pool;
MainRenderer renderer;
LinkManager lnkmgr;
Camera* cam = pool.createCamera();

pthread_t callThd[2];
pthread_mutex_t mutex;

void init() {
	window.init_SDL();
	window.GL_init();
	pool.mutex = &mutex;
	renderer.pool = &pool;
	lnkmgr.pool = &pool;
	lnkmgr.rm = new plResManager;
	lnkmgr.Load("C:\\Personal_District_psnlMYSTII.prp");
	renderer.UpdateList(false);
}

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


void* DrawFunc(void* arg) {
	init();
	while (1) {
		ProcessEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.draw();

		pthread_mutex_lock(&mutex);
		cam->turn();
		cam->update();
		pthread_mutex_unlock(&mutex);
		SDL_GL_SwapBuffers();
	}
	pthread_exit(0);
	return 0;
}

void* UpdatePos(void* arg) {
	while (1) {
		Sleep(10);
		pthread_mutex_lock(&mutex);
		if (cam->angle > 365.0f)
			cam->angle = 0.0f;
		cam->angle += 0.001f;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
	return 0;
}

int main(int argc, char** argv) {
	void *status;
	pthread_mutex_init(&mutex, NULL);

	if (pthread_create(&callThd[0], NULL, DrawFunc, NULL))
		return 0;
	if (pthread_create(&callThd[1], NULL, UpdatePos, NULL))
		return 0;
	while (1) {
		ProcessEvents();
	}
	pthread_mutex_destroy(&mutex);
	return 1;
}
