#include "plglGame.h"

plglGame::plglGame(plResManager* rm) {
    this->rm = rm;
    spawnmgr.init(rm);
}
