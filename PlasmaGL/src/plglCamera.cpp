#include "plglCamera.h"

const float RADS = 0.0174532925f;

plglCamera::plglCamera() {
    position[0] = 0.0f;
    position[1] = 0.0f;
    position[2] = 0.0f;
    zrotation = 0.0f;
}

plglCamera::~plglCamera() {}

float* plglCamera::GetPosition() { return position; }
void plglCamera::setZRotation(float rotation) {
    zrotation = rotation;
}

void plglCamera::SetPositionX(float position) { this->position[0] = position; }
void plglCamera::SetPositionY(float position) { this->position[1] = position; }
void plglCamera::SetPositionZ(float position) { this->position[2] = position; }

void plglCamera::MoveForward(float amount) {
    position[0] += (sinf(zrotation*RADS)*amount);
    position[1] += (cosf(zrotation*RADS)*amount);
}
void plglCamera::zrotate(float degrees) {
    zrotation = zrotation + degrees;
    if (zrotation >= 360) {
        zrotation = zrotation-360;
    }
    else if (zrotation < 0) {
        zrotation = 360+zrotation;
    }
}
float* plglCamera::GetZRotation() { return &zrotation; }
void plglCamera::MoveZ(float amount) { position[2]+=amount; }
