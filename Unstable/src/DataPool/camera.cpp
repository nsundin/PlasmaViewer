#include "camera.h"

Camera::Camera() {
	this->camFOV=60.0;
}
void Camera::turn() {
    camL[0] = sin(this->angle);
    camL[1] = cos(this->angle);
}

void Camera::moveLocalY(float speed) {
    cam[2] = cam[2] + speed;
}
void Camera::moveLocalZ(float speed) {
    cam[0] = cam[0] + (camL[0])*speed;
    cam[1] = cam[1] + (camL[1])*speed;
}

void Camera::warp(float x,float y,float z) {
	cam[0] = x;
	cam[1] = y;
	cam[2] = z;
}

void Camera::update() {
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam[0], cam[1], cam[2], cam[0] + camL[0], cam[1] + camL[1], cam[2] + camL[2], 0.0f,0.0f,1.0f);

}

float Camera::getCamPos(int i) {
	return cam[i];
}
float Camera::getCamPosL(int i) {
	return camL[i];
}


