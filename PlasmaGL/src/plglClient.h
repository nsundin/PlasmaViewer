#ifndef PLGLCLIENT_H
#define PLGLCLIENT_H

#include "ResManager/plResManager.h"
#include "Debug/plDebug.h"
#include "plglRenderer.h"
#include "plglGame.h"
#include "plglCamera2.h"

class plglClient {
public:
    plglClient(const char *startage);
    void doMain();
private:
    void InitDisplay();
    void ProcessEvents(Uint32 timefactor);
    plglCamera2 camera;
    plglGame* game;
    plglRenderer* renderer;
    plResManager* rm;
    SDL_Event event;

    int mouseX;
    int mouseY;
    int oldmouseX;
    int oldmouseY;
};

#endif
