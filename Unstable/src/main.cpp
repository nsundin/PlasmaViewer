#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include "DataPool/DataPool.h"
#include "Draw/MainRenderer.h"
#include "SDLWindow.h"
#include "PlasmaDefs.h"
#include "ResManager/plResManager.h"
#include "Control/LinkManager.h"
#include "Msg/MessageExecuter.h"

#define FRAME_WAIT         20 //no I didn't mean frame-rate ;)
#define MSG_EXECUTER_RATE  20

bool isRendering = false;
bool isUpdate = false;

DataPool pool;
MainRenderer renderer;
LinkManager lnkmgr;
Camera* cam = pool.createCamera();
Player* currPlayer = new Player;
SDLWindow window(&pool);
MessageExecuter msgexe;

pthread_t callThd[2];
pthread_mutex_t mutex;

//const char* StartUpPath = "C:\\Program Files\\CyanWorlds\\UruCC\\dat\\city.age";
const char* StartUpPath = "C:\\Kveer.age";


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

	window.init_SDL();
	window.GL_init();
//	cam->warp(0.0f,0.0f,15.0f);
	while (1) {
		pthread_mutex_lock(&mutex);
		now_time = SDL_GetTicks();
		timefactor = now_time-last_time;
		last_time = now_time;
		if (isUpdate) {
			renderer.UpdateList(false);
			cam->turn();
			isUpdate = false;
		}
		if (isRendering) {
			MotionHandler(timefactor);
			window.GLDraw(&renderer);
			window.ProcessEvents();
		}
		pthread_mutex_unlock(&mutex);
		SDL_Delay(FRAME_WAIT);
	}
	pthread_exit(0);
	return 0;
}

void* MsgExeLoop(void* arg) {
	while (1) {
		Sleep(MSG_EXECUTER_RATE);
		for (size_t i=0; i < pool.getNumMsgs(); i++) {
			pthread_mutex_lock(&mutex);
			printf("msgrcvd:%i\n",pool.getMessage(i));
			msgexe.HandleMessage(pool.getMessage(i));
			if (pool.getMessage(i)->MsgUseage == EngineMessage::kLocalOneUse) {
				printf("msgdel:%i\n",pool.getMessage(i));
				pool.DeleteMessageByInd(i);
			}
			pthread_mutex_unlock(&mutex);
		}
	}
	pthread_exit(0);
	return 0;
}

void testMsgCommand() {
	pthread_mutex_lock(&mutex);
	EngineMessage* msg = new EngineMessage;
	msg->MsgType = EngineMessage::kLinkMgrLoad;
	msg->chars01 = (char*)StartUpPath;
	msg->MsgUseage = EngineMessage::kLocalOneUse;
	pool.RegisterMessage(msg);
	pthread_mutex_unlock(&mutex);
	printf("sent test msg\n");
}

void printNumMessagesCommand() {
	pthread_mutex_lock(&mutex);
	printf("Num Messages to be executed: %i\n",pool.getNumMsgs());
	pthread_mutex_unlock(&mutex);
}

void startRendering() {
	pthread_mutex_lock(&mutex);
	isRendering = true;
	printf("\nRendering started\n");
	pthread_mutex_unlock(&mutex);
}

void endRendering() {
	pthread_mutex_lock(&mutex);
	isRendering = false;
	printf("\nRendering ended\n");
	pthread_mutex_unlock(&mutex);
}

void updateRendering() {
	pthread_mutex_lock(&mutex);
	isUpdate = true;
	printf("\nRendering updating\n");
	pthread_mutex_unlock(&mutex);
}


int main(int argc, char** argv) {
	if (argc > 1) {
		StartUpPath = argv[1];
	}
	pthread_mutex_init(&mutex, NULL);

	plString fontpath =  (plString(argv[0]).beforeLast(PATHSEP)+PATHSEP)+plString("FreeMono.ttf");
	window.initConsole(fontpath.cstr());
	msgexe.lnkmgr = &lnkmgr;
	msgexe.renderer = &renderer;
	pool.mutex = &mutex;
	renderer.pool = &pool;
	lnkmgr.pool = &pool;
	lnkmgr.rm = new plResManager;
	pool.activePlayer = currPlayer;
	pool.SetCurrentCamera(cam);

	if (pthread_create(&callThd[0], NULL, SDLFunc, NULL))
		return 0;
	if (pthread_create(&callThd[1], NULL, MsgExeLoop, NULL))
		return 0;

	//do init thing (send load message start engine etc.)
	testMsgCommand();
	updateRendering();
	startRendering();
	//maybe this could be a Python command-prompt
	while (1) {
		std::string command;
		printf("> ");
		getline(std::cin, command);
		if (command == (std::string)"") {}
		else if (command == (std::string)"sendtestmsg") {
			testMsgCommand();
		}
		else if (command == (std::string)"nummessages") {
			printNumMessagesCommand();
		}
		else if (command == (std::string)"updater") {
			updateRendering();
		}
		else if (command == (std::string)"startr") {
			startRendering();
		}
		else if (command == (std::string)"endr") {
			endRendering();
		}
		else if (command == (std::string)"init") {
			testMsgCommand();
			updateRendering();
			startRendering();
		}
		else if (command == (std::string)"warp") {
			pthread_mutex_lock(&mutex);
			cam->warp(111.0f,22.0f,47.0f);
			pthread_mutex_unlock(&mutex);
		}
		else {
			printf("No command '%s'\n",command.c_str());
		}
	}
	pthread_mutex_destroy(&mutex);
	return 1;
}
