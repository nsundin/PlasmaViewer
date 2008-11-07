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


void* DrawFunc(void* arg) {
	init();
	while (1) {
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
		pthread_mutex_lock(&mutex);
		if (cam->angle > 365.0f)
			cam->angle = 0.0f;
		cam->angle += 0.01f;
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
//	if (pthread_create(&callThd[1], NULL, UpdatePos, NULL))
//		return 0;

	pthread_mutex_destroy(&mutex);
	pthread_exit(NULL);
	return 1;
}
