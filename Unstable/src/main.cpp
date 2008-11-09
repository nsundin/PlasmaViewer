#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include "DataPool/DataPool.h"
#include "Draw/MainRenderer.h"
#include "SDLWindow.h"
#include "ResManager/plResManager.h"
#include "Control/LinkManager.h"

DataPool pool;
MainRenderer renderer;
LinkManager lnkmgr;
Camera* cam = pool.createCamera();
Player* currPlayer = new Player;
SDLWindow window(&pool);

pthread_t callThd[2];
pthread_mutex_t mutex;

void init() {
	window.init_SDL();
	window.GL_init();
	pool.mutex = &mutex;
	renderer.pool = &pool;
	lnkmgr.pool = &pool;
	lnkmgr.rm = new plResManager;
	lnkmgr.Load("C:\\Program Files\\CyanWorlds\\UruCC\\dat\\Personal.age");
	renderer.UpdateList(false);
	cam->warp(0.0f,0.0f,15.0f);
}

void MotionHandler(Uint32 TimeFactor) {
    if (currPlayer->isMovingForward) {
        if (currPlayer->isRun) {
            cam->moveLocalZ(0.08f*TimeFactor);
        }
        else {
            cam->moveLocalZ(0.03f*TimeFactor);
        }
    }
    if (currPlayer->isMovingBackward) {
        cam->moveLocalZ(-0.03f*TimeFactor);
    }
    if (currPlayer->isTurningRight) {
        cam->angle += 0.003f*TimeFactor;
        cam->turn();
    }
    if (currPlayer->isTurningLeft) {
        cam->angle -= 0.003f*TimeFactor;
        cam->turn();
    }
    if (currPlayer->isMovingUp) {
        cam->moveLocalY(0.03f*TimeFactor);
    }
    if (currPlayer->isMovingDown) {
        cam->moveLocalY(-0.03f*TimeFactor);
    }
}

void* SDLFunc(void* arg) {
	Uint32 last_time;
	Uint32 now_time;
	Uint32 timefactor;
	init();
	window.GLDraw(&renderer,cam);
	cam->turn();
	while (1) {
		now_time = SDL_GetTicks();
		timefactor = now_time-last_time;
		last_time = now_time;
//		printf("%i\n",timefactor);
		MotionHandler(timefactor);
		window.GLDraw(&renderer,cam);
		window.ProcessEvents();
	}
	pthread_exit(0);
	return 0;
}

void* UpdatePos(void* arg) {
	while (1) {
		Sleep(5);
		pthread_mutex_lock(&mutex);
//		MotionHandler();
//		if (cam->angle > 365.0f)
//			cam->angle = 0.0f;
//		cam->angle += 0.001f;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
	return 0;
}

int main(int argc, char** argv) {
	pool.activePlayer = currPlayer;
	pthread_mutex_init(&mutex, NULL);

	if (pthread_create(&callThd[0], NULL, SDLFunc, NULL))
		return 0;
//	if (pthread_create(&callThd[1], NULL, UpdatePos, NULL))
//		return 0;
	//maybe this could be a Python command-prompt
	while (1) {
		std::string command;
		printf("> ");
		getline(std::cin, command);
		if (command == (std::string)"") {}
		else if (command == (std::string)"some command") {
			//do something
		}
		else {
			std::cout << command << std::endl;
		}
	}
	pthread_mutex_destroy(&mutex);
	return 1;
}
