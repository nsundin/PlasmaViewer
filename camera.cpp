#include "camera.h"

camera::camera() {
	this->camFOV=60.0;
}
void camera::turn() {
    camL[0] = sin(this->angle);
    camL[1] = cos(this->angle);
	this->update();
}

void camera::moveLocalY(float speed) {
    cam[2] = cam[2] + speed;
	this->update();
}
void camera::moveLocalZ(float speed) {
    cam[0] = cam[0] + (camL[0])*speed;
    cam[1] = cam[1] + (camL[1])*speed;
	this->update();
}

void camera::warp(float x,float y,float z) {
	cam[0] = x;
	cam[1] = y;
	cam[2] = z;
	this->update();
}

void camera::update() {
    glLoadIdentity();
    gluLookAt(cam[0], cam[1], cam[2], cam[0] + camL[0], cam[1] + camL[1], cam[2] + camL[2], 0.0f,0.0f,1.0f);

}
float camera::getCamPos(int i) {
	return cam[i];
}
float camera::getCamPosL(int i) {
	return camL[i];
}
