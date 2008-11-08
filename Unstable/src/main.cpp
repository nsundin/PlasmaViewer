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


void* SDLFunc(void* arg) {
	init();
	while (1) {
		window.ProcessEvents();
		window.GLDraw(&renderer,cam);
	}
	pthread_exit(0);
	return 0;
}

void* UpdatePos(void* arg) {
	while (1) {
		Sleep(5);
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
	pthread_mutex_init(&mutex, NULL);

	if (pthread_create(&callThd[0], NULL, SDLFunc, NULL))
		return 0;
	if (pthread_create(&callThd[1], NULL, UpdatePos, NULL))
		return 0;
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
