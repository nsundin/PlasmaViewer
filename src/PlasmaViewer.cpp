#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#else
#include <GL/glx.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#include "ResManager/plResManager.h"
#include "ResManager/plAgeInfo.h"
#include "Debug/hsExceptions.h"
#include "Debug/plDebug.h"
#include "PlasmaDefs.h"
#include "prpengine.h"
#include "window.h"
#include "player.h"

std::vector<plKey> GlobalSceneObjectList;

int vpaint = 1;
bool wireframe = false;

window SDLWindow;
plResManager rm;
camera cam;
Player currentPlayer;
prpengine prp_engine;


void GL_init(const char* filename, std::vector<plKey> SObjects) {
    glDisable(GL_LIGHTING);
    glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
//	prp_engine.AttemptToSetFniSettings(plString(filename));
    prp_engine.AttemptToSetPlayerToLinkPointDefault(SObjects,cam);
}

void KeyDownTrue(bool* var, unsigned press_type) {
	if (press_type == SDL_KEYDOWN)
		*var = true;
	else
		*var = false;
}
static void KeyCallback(SDL_keysym* keysym,unsigned int type) {
    switch(keysym->sym) {
        case SDLK_ESCAPE:
            if (type == SDL_KEYDOWN) SDLWindow.quit(0);
            break;
 //Player Control Keys
        case SDLK_LEFT:
            KeyDownTrue(&currentPlayer.isTurningLeft,type);
            break;
        case SDLK_RIGHT:
            KeyDownTrue(&currentPlayer.isTurningRight,type);
			break;
        case SDLK_UP:
            KeyDownTrue(&currentPlayer.isMovingForward,type);
            break;
        case SDLK_DOWN:
			KeyDownTrue(&currentPlayer.isMovingBackward,type);
            break;
        case SDLK_x:
			KeyDownTrue(&currentPlayer.isMovingDown,type);
            break;
        case SDLK_z:
			KeyDownTrue(&currentPlayer.isMovingUp,type);
            break;
        case SDLK_LSHIFT:
			KeyDownTrue(&currentPlayer.isRun,type);
            break;
        case SDLK_RSHIFT:
			KeyDownTrue(&currentPlayer.isRun,type);
            break;
//Other Keys
        case SDLK_d:
            if (type == SDL_KEYDOWN)
                printf("You are facing %f rads\n",cam.angle);
            break;
        case SDLK_l:
            if (type == SDL_KEYDOWN)
                printf("You are at (%f, %f, %f)\n",cam.getCamPos(0), cam.getCamPos(1),cam.getCamPos(2));
            break;
        case SDLK_p:
            if (type == SDL_KEYDOWN)
                prp_engine.PrintObjects();
            break;
        case SDLK_s:
            if (type == SDL_KEYDOWN) {
                if (currentPlayer.isRun) prp_engine.PrevSpawnPoint(cam);
                else prp_engine.NextSpawnPoint(cam);
            }
            break;
        case SDLK_w:
            if (type == SDL_KEYDOWN){
                wireframe = !wireframe;
                prp_engine.UpdateList(wireframe,true,cam);
            }
            break;
        default:
            break;
    }
}

void ProcessEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            KeyCallback(&event.key.keysym,event.type);
//            if (event.key.keysym.sym>27){
//                std::cout << char(event.key.keysym.unicode);
//            }
            break;
        }
        if (event.type == SDL_VIDEORESIZE) {
            SDLWindow.window_w = event.resize.w;
            SDLWindow.window_h = event.resize.h;
            SDLWindow.resize();
            break;
        }
        if (event.type == SDL_QUIT) {
            SDLWindow.quit(0);
            break;
        }
    }
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
//	std::cout << SDL_GetTicks() << std::endl;
	prp_engine.draw(cam);
    cam.update();
//    glFlush();
    SDL_GL_SwapBuffers();
}


void drawLoading(float loaded, float loaded2) {
    const float inset = 48.0f;
    static float primary = 0.0, secondary = 0.0;
    if(loaded >= 0.0) primary = loaded;
    if(loaded2 >= 0.0) secondary = loaded2;
//    printf("drawLoading: %f/%f\n",primary,secondary);
    glViewport(0, 0, SDLWindow.window_w, SDLWindow.window_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)SDLWindow.window_w, 0.0,(double)SDLWindow.window_h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.27f,0.46f,0.33f);

    float fullbarwidth = SDLWindow.window_w-inset*2.0-4.0; //foobarwidth anyone?
    float barwidth = fullbarwidth*primary;
    float barwidth2 = fullbarwidth*secondary;

	glRectf(inset+2.0f, 67.0f, inset+2.0+barwidth, 52.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(inset,68.0f);
    glVertex2f(SDLWindow.window_w-inset-1.0,68.0f);
    glVertex2f(SDLWindow.window_w-inset-1.0,50.0f);
    glVertex2f(inset,50.0f);
    glEnd();
    if(secondary > 0.0) {
        glRectf(inset+2.0f, 97.0f, inset+2.0+barwidth2, 82.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(inset,98.0f);
        glVertex2f(SDLWindow.window_w-inset-1.0,98.0f);
        glVertex2f(SDLWindow.window_w-inset-1.0,80.0f);
        glVertex2f(inset,80.0f);
        glEnd();
    }
    SDL_GL_SwapBuffers();
    ProcessEvents();
}
void drawSecondaryProgress(float progress) {
    drawLoading(-1.0, progress);
}

void MotionHandler() {
//currently hooked right up with the camera for first person
    if (currentPlayer.isMovingForward) {
        if (currentPlayer.isRun) {
            cam.moveLocalZ(1.0f);
        }
        else {
            cam.moveLocalZ(0.45f);
        }
    }
    if (currentPlayer.isMovingBackward) {
        cam.moveLocalZ(-0.45f);
    }
    if (currentPlayer.isTurningRight) {
        cam.angle += 0.025f;
        cam.turn();
    }
    if (currentPlayer.isTurningLeft) {
        cam.angle -= 0.025f;
        cam.turn();
    }
    if (currentPlayer.isMovingUp) {
        cam.moveLocalY(0.45f);
    }
    if (currentPlayer.isMovingDown) {
        cam.moveLocalY(-0.45f);
    }
}

void LoadLocation(const plLocation &loc) {
	prp_engine.LoadTextures(rm.getKeys(loc, kMipmap));
    prp_engine.AppendObjectsToDrawList(rm.getKeys(loc, kSceneObject));
	prp_engine.AppendClustersToDrawList(rm.getKeys(loc, kClusterGroup));

	std::vector<plKey> PageScnObjs = rm.getKeys(loc, kSceneObject);
    for (size_t i = 0; i < PageScnObjs.size(); i++) {
		GlobalSceneObjectList.push_back(PageScnObjs[i]);
	}
}

int Load(const char* filename) {
    printf("Loading Scene... use arrow keys to move around and Z and X to move up and down\n\n");
    if (plString(filename).afterLast('.') == "age") {
        plAgeInfo* age = rm.ReadAge(filename, false);

        //read the pages... because of the progress bar we can't leave this up to the resMgr with ReadAge
        plString path = plString(filename).beforeLast(PATHSEP);
        if (path.len() > 0)
            path = path + PATHSEP;

        if (age->getNumPages() > 0) {
            plString file = plString::Format("%s_District_%s.prp",
                    age->getAgeName().cstr(),
                    age->getPage(0).fName.cstr());
            FILE* F = fopen((path + file).cstr(), "rb");
            if (F == NULL) {
                rm.setVer(pvEoa, true);
            } else {
                rm.setVer(pvPots, true);
                fclose(F);
            }
        }
        int num_total_pages = age->getNumPages();
        drawLoading(0.0, 0.0);
        ProgressCallback old = rm.SetProgressFunc(&drawSecondaryProgress);
        for (size_t i=0; i<age->getNumPages(); i++) {
            rm.ReadPage(path + age->getPageFilename(i, rm.getVer()));
            float completed = ((float)i+1.0f)/(float)num_total_pages;
            drawLoading(completed, 0.0);
        }
        for (size_t i=0; i<age->getNumCommonPages(rm.getVer()); i++) {
            rm.ReadPage(path + age->getCommonPageFilename(i, rm.getVer()));
        }
        rm.SetProgressFunc(old);
        //end of page-reading

        for (size_t i1 = 0; i1 < age->getNumPages(); i1++) {
            LoadLocation(age->getPageLoc(i1,rm.getVer()));
        }
        for (size_t i1 = 0; i1 < age->getNumCommonPages(rm.getVer()); i1++) {
            LoadLocation(age->getCommonPageLoc(i1,rm.getVer()));
        }
    }
    else if (plString(filename).afterLast('.') == "prp") {
        LoadLocation(rm.ReadPage(filename)->getLocation());
        plString base = plString(filename).beforeLast('_');
        try {
            plString texs = base + plString("_Textures.prp");
            LoadLocation(rm.ReadPage(texs)->getLocation());
        }
        catch(...) {}
    }
    return 1;
}

int main(int argc, char* argv[]) {
	SDLWindow.init_SDL();

    plDebug::Init(plDebug::kDLAll);

    //drawLoading(0.0f); //give 'em something to look at before the first page loads
    if (argc < (int) 2) {
		Load("C:\\Personal_District_psnlMYSTII.prp");
        printf("expects prp-path as first argument, loading default\n");
//		return 0;
    }
	else {
        Load(argv[1]);
	}
    prp_engine.UpdateList(wireframe,true,cam);

    GL_init(argv[1],GlobalSceneObjectList);
	SDLWindow.resize();
	while(1) {
        ProcessEvents();
        MotionHandler();
        draw();
    }
    return 0;
}
