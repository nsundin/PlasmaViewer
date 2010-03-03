#ifndef PLGLGAME_H
#define PLGLGAME_H

#include "plglSpawnPtMgr.h"

class plglGame {
public:
    plglGame(plResManager* rm);
    plglSpawnPtMgr spawnmgr;
private:
    plResManager* rm;
};

#endif
