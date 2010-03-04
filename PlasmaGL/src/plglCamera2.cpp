#include "plglCamera2.h"
#include <SDL_opengl.h>

const float RADS = 0.0174532925f;

plglCamera2::plglCamera2() {
    this->camFOV=60.0;
}
void plglCamera2::turn(float angle) {
    this->angle += angle;
    camL[0] = sinf(this->angle*RADS);
    camL[1] = cosf(this->angle*RADS);
}

void plglCamera2::moveLocalY(float speed) {
    cam[2] = cam[2] + speed;
}
void plglCamera2::moveLocalZ(float speed) {
    cam[0] = cam[0] + (camL[0])*speed;
    cam[1] = cam[1] + (camL[1])*speed;
}

void plglCamera2::warp(float x,float y,float z) {
    cam[0] = x;
    cam[1] = y;
    cam[2] = z;
}

void plglCamera2::update() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam[0], cam[1], cam[2], cam[0] + camL[0], cam[1] + camL[1], cam[2], 0.0f,0.0f,1.0f);

}

float plglCamera2::getCamPos(int i) {
    return cam[i];
}
float plglCamera2::getCamPosL(int i) {
    return camL[i];
}
