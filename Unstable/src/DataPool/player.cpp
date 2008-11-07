#include "player.h"

void Player::SetStill() {
    isMovingForward = false;
    isMovingBackward = false;
    isTurningRight = false;
    isTurningLeft = false;
    isMovingUp = false;
    isMovingDown = false;
    isRun = false;
}