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
#include "Util/plString.h"
#include "Util/hsTArray.hpp"
#include "Math/hsMatrix44.h"

#include "PRP/Object/plSceneObject.h"

#include "PRP/Object/plCoordinateInterface.h"

#include "PRP/Surface/hsGMaterial.h"
#include "PRP/Surface/hsGMatState.h"
#include "Sys/hsColor.h"
#include "prpengine.h"

PlasmaVer ver = pvPots;
int ageLoadMode = 0;

int vpaint = 1;
//ScreenTextMgr* textmgr;
//NAV
bool isMovingForward = false;
bool isMovingBackward = false;
bool isTurningRight = false;
bool isTurningLeft = false;
bool isMovingUp = false;
bool isMovingDown = false;
bool isShift = false;
bool wireframe = false;
//End of NAV


//windowInfo
int window_bpp = 0;
int window_flags = 0;
int window_w = 1;
int window_h = 1;
//end of window info


plResManager rm;

plPageInfo* page;

const char* filename;

hsTArray<plKey> SObjects;
hsTArray<plKey> Textures;

camera cam;
prpengine prp_engine;

void quit(int code) {
    SDL_Quit();
    exit(code);
}
void init() {
//    glEnable(GL_LINE_SMOOTH);
    glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);	
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    if (ageLoadMode) {
//        prp_engine.AttemptToSetFniSettings(plString(filename));
    }
    prp_engine.AttemptToSetPlayerToLinkPointDefault(SObjects,cam);
//	textmgr->printToScreen("Hello World!");
}
void resize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(cam.camFOV, (double)w / (double)h, 1.0f, 100000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    window_w = w;
    window_h = h;
	cam.update();
}

void draw() {
//	glDisable(GL_DEPTH_TEST);
    //glEnable (GL_POLYGON_SMOOTH);
    glMatrixMode(GL_MODELVIEW);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	glEnable(GL_POLYGON_OFFSET_FILL);
//	glPolygonOffset(1.0, 1.0);

	prp_engine.draw();
//	textmgr->Render(window_w,window_h);
    SDL_GL_SwapBuffers();
}

void setVideoMode(int w,int h,int flags,int bpp) {
    if( SDL_SetVideoMode(w, h, bpp, flags) == 0 ) {
        printf("[FAIL] SDL_SetVideoMode: %s\n", SDL_GetError());
        quit(1);
    }
}

void MotionHandler() {
    if (isMovingForward) {
        if (isShift) {
            cam.moveLocalZ(1.0f);
        }
        else {
            cam.moveLocalZ(0.45f);
        }
    }
    if (isMovingBackward) {
        cam.moveLocalZ(-0.45f);
    }
    if (isTurningRight) {
        cam.angle += 0.025f;
        cam.turn();
    }
    if (isTurningLeft) {
        cam.angle -= 0.025f;
        cam.turn();
    }
    if (isMovingUp) {
        cam.moveLocalY(0.45f);
    }
    if (isMovingDown) {
        cam.moveLocalY(-0.45f);
    }
}

static void KeyCallback(SDL_keysym* keysym,unsigned int type) {
    switch(keysym->sym) {
        case SDLK_ESCAPE:
            if (type == SDL_KEYDOWN) quit(0);
            break;
        case SDLK_LEFT: 
            if (type == SDL_KEYDOWN) isTurningLeft = true;
            else isTurningLeft = false;
            break;
        case SDLK_RIGHT:
            if (type == SDL_KEYDOWN) isTurningRight = true;
            else isTurningRight = false;
            break;
        case SDLK_UP:
            if (type == SDL_KEYDOWN) isMovingForward = true;
            else isMovingForward = false;
            break;
        case SDLK_DOWN:
            if (type == SDL_KEYDOWN) isMovingBackward = true;
            else isMovingBackward = false;
            break;
        case SDLK_d:
            if (type == SDL_KEYDOWN)
                printf("You are facing %f rads\n",cam.angle);
            break;
        case SDLK_l:
            if (type == SDL_KEYDOWN)
                printf("You are at (%f, %f, %f)\n",cam.getCamPos(0), cam.getCamPos(1),cam.getCamPos(2));
            break;
        case SDLK_w:
            if (type == SDL_KEYDOWN){
                wireframe = !wireframe;
                prp_engine.UpdateList(SObjects,wireframe);
            }
            break;
        case SDLK_x:
            if (type == SDL_KEYDOWN) isMovingDown = true;
            else isMovingDown = false;
            break;
        case SDLK_z:
            if (type == SDL_KEYDOWN) isMovingUp = true;
            else isMovingUp = false;
            break;
        case SDLK_LSHIFT:
            if (type == SDL_KEYDOWN) isShift = true;
            else isShift = false;
            break;
        case SDLK_RSHIFT:
            if (type == SDL_KEYDOWN) isShift = true;
            else isShift = false;
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
            break;
        }
   //     if (event.type == SDL_VIDEORESIZE) {
			//printf("RESIZE!!!\n");
   //         window_w = event.resize.w;
   //         window_h = event.resize.h;
   //         setVideoMode(window_w,window_h,window_flags,window_bpp);
   //         resize(window_w, window_h);
   //         break;
   //     }
        if (event.type == SDL_QUIT) {
            quit(0);
            break;
        }
    }
}

void appendTexture(plKey texturekey) {
    if (texturekey.Exists() && texturekey.isLoaded()) {
        Textures.append(texturekey);
    }
}

void appendSObj(plKey sobjectkey) {
    if (sobjectkey.Exists() && sobjectkey.isLoaded()) {
        SObjects.append(sobjectkey);
    }
}
 

int Load(int argc, char** argv) {
  rm.setVer(ver, true);
  plDebug::Init(plDebug::kDLAll);
  if (argc < (int) 2) {
//	filename = "C:\\test.prp";
      filename = "C:\\Kveer.age";
    printf("expects prp-path as first argument\n");
//  return 0;
  }
  else {
  filename = argv[1];
  std::cout << filename << std::endl;
  }
  char somevar = filename[0];
  printf("Loading Scene... use arrow keys to move around and Z and X to move up and down\n\n");
  try {
     if (plString(filename).afterFirst('.') == "age") {
        ageLoadMode = 1;
        plAgeInfo* age = rm.ReadAge(filename, true);
        for (size_t i1 = 0; i1 < age->getNumPages(); i1++) {
            std::vector<plKey> mipkeys = rm.getKeys(age->getPageLoc(i1,ver), kMipmap);
            std::vector<plKey> so_s = rm.getKeys(age->getPageLoc(i1,ver), kSceneObject);
            for (size_t i = 0; i < mipkeys.size(); i++) {
                appendTexture(mipkeys[i]);
            }
            for (size_t i = 0; i < so_s.size(); i++) {
                appendSObj(so_s[i]);
            }
        }
        for (size_t i1 = 0; i1 < age->getNumCommonPages(ver); i1++) {
            std::vector<plKey> mipkeys = rm.getKeys(age->getCommonPageLoc(i1,ver), kMipmap);
            std::vector<plKey> so_s = rm.getKeys(age->getCommonPageLoc(i1,ver), kSceneObject);
            for (size_t i = 0; i < mipkeys.size(); i++) {
                appendTexture(mipkeys[i]);
            }
            for (size_t i = 0; i < so_s.size(); i++) {
                appendSObj(so_s[i]);
            }
        }
     }
     else if (plString(filename).afterFirst('.') == "prp") {
        page = rm.ReadPage(filename);
        std::vector<plKey> mipkeys = rm.getKeys(page->getLocation(), kMipmap);
        std::vector<plKey> so_s = rm.getKeys(page->getLocation(), kSceneObject);
        for (size_t i = 0; i < mipkeys.size(); i++) {
            appendTexture(mipkeys[i]);
        }
        for (size_t i = 0; i < so_s.size(); i++) {
            appendSObj(so_s[i]);
        }
     }
  } catch (const hsException& e) {
      plDebug::Error("%s:%lu: %s", e.File(), e.Line(), e.what());
      return 0;
  } catch (const std::exception& e) {
      plDebug::Error("%s", e.what());
      return 0;
  } catch (...) {
      plDebug::Error("Undefined error!");
      return 0;
  }
  return 1;
}



int main(int argc, char* argv[]) {
//int _tmain(int argc, char** argv) {
    Load(argc, argv);
    const SDL_VideoInfo* info = NULL;

    if(SDL_Init(SDL_INIT_VIDEO ) < 0) {
        printf("[FAIL] SDL_INIT_VIDEO: %s\n", SDL_GetError());
        quit(1);
    }

    info = SDL_GetVideoInfo();
    if(!info) {
        printf("[FAIL] SDL_GetVideoInfo: %s\n",SDL_GetError());
        quit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_WM_SetCaption("PlasmaViewer", "PlasmaViewer");


    window_w = 800;//1280;
    window_h = 600;//1024;
    window_flags = SDL_OPENGL;// | SDL_RESIZABLE;// | SDL_NOFRAME | //SDL_FULLSCREEN;
    window_bpp = info->vfmt->BitsPerPixel;
    setVideoMode(window_w,window_h,window_flags,window_bpp);

    prp_engine.LoadAllTextures(Textures);
    prp_engine.AppendAllObjectsToDrawList(SObjects);
//    SortDrawableList();
    prp_engine.UpdateList(SObjects,wireframe);

    resize(window_w, window_h);
    init();
    while(1) {
        ProcessEvents();
        MotionHandler();
        draw();
    }
    return 0;
}


