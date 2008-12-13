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
#include "Physics/PhysicsEngine.h"

#define FRAME_WAIT         10 //no I didn't mean frame-rate ;)
#define MSG_EXECUTER_RATE  90

bool isRendering = false;
bool isUpdate = false;
bool isPhysicsUpdate = false;
bool isPhysicsEnabled = false;

DataPool pool;
MainRenderer renderer;
LinkManager lnkmgr;
Camera* cam = pool.createCamera();
Player* currPlayer = new Player;
SDLWindow window(&pool);
MessageExecuter msgexe;
PhysicsEngine phys;

pthread_t callThd[2];
pthread_mutex_t mutex;

//const char* StartUpPath = "C:\\Program Files\\CyanWorlds\\UruCC\\dat\\city.age";
const char* StartUpPath = "C:\\Documents and Settings\\Owner\\Desktop\\PlasmaViewer\\coll_test.age";


void MotionHandler(Uint32 TimeFactor) {
	pthread_mutex_lock(&mutex);
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
	pthread_mutex_unlock(&mutex);
}

void* SDLFunc(void* arg) {
	Uint32 last_time;
	Uint32 now_time;
	Uint32 timefactor;

	window.init_SDL();
	window.GL_init();
	phys.initWorldAndEngine();
	cam->warp(0.0f,0.0f,0.0f);
	while (1) {
//		printf("SDLFunc\n");
		now_time = SDL_GetTicks();
		timefactor = now_time-last_time;
		last_time = now_time;

		pthread_mutex_lock(&mutex);
		if (isUpdate) {
			renderer.UpdateList(false);
			cam->turn();
			isUpdate = false;
		}
		if (isPhysicsUpdate) {
			phys.LoadAllObjectsIntoCalc(pool.PhysicalObjects);
			isPhysicsUpdate = false;
		}
		pthread_mutex_unlock(&mutex);
		
		if (isRendering) {
//			phys.CalulateCollisions();
			MotionHandler(timefactor);
			window.GLDraw(&renderer);
			window.ProcessEvents();
			if (isPhysicsEnabled) {
				phys.updateObjectMatrices();
				phys.ClearAndStepEngine(FRAME_WAIT);
			}
		}
		SDL_Delay(FRAME_WAIT);
	}
	pthread_exit(0);
	return 0;
}

void* MsgExeLoop(void* arg) {
	while (1) {
		Sleep(MSG_EXECUTER_RATE);
		pthread_mutex_lock(&mutex);
		for (size_t i=0; i < pool.getNumMsgs(); i++) {
			printf("msgrcvd:%i\n",pool.getMessage(i));
			msgexe.HandleMessage(pool.getMessage(i));
			if (pool.getMessage(i)->MsgUseage == EngineMessage::kLocalOneUse) {
				printf("msgdel:%i\n",pool.getMessage(i));
				pool.DeleteMessageByInd(i);
			}
		}
		pthread_mutex_unlock(&mutex);
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
	lnkmgr.window = &window;
	msgexe.lnkmgr = &lnkmgr;
	msgexe.renderer = &renderer;
	pool.mutex = &mutex;
	renderer.pool = &pool;
	phys.pool = &pool;
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
	pthread_mutex_lock(&mutex);
//	lnkmgr.Load((char*)StartUpPath);
	pthread_mutex_unlock(&mutex);

	startRendering();

	updateRendering();
	
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
		else if (command == (std::string)"phys") {
			isPhysicsUpdate = true;
			isPhysicsEnabled = true;
			printf("updated and enabled physics\n");
		}
		else if (command == (std::string)"nophys") 
			isPhysicsEnabled = false;
		else if (command == (std::string)"linkinpos") {
			pthread_mutex_lock(&mutex);
			hsMatrix44 mat = pool.getLinkInPointMatrix();
			printf("%f, %f, %f\n",mat(0,3),mat(1,3),mat(2,3));
			pthread_mutex_unlock(&mutex);
		}
		else if (command == (std::string)"linkin") {
			pthread_mutex_lock(&mutex);
			hsMatrix44 mat = pool.getLinkInPointMatrix();
			cam->warp(mat(0,3),mat(1,3),mat(2,3));
			pthread_mutex_unlock(&mutex);
		}
		else {
			printf("No command '%s'\n",command.c_str());
		}
	}
	phys.Unload();
	pthread_mutex_destroy(&mutex);
	return 1;
}
