#ifndef PLGLCAMERA_H
#define PLGLCAMERA_H

#include <math.h>

class plglCamera {
public:
    plglCamera();
    ~plglCamera();
    float* GetPosition();
    void SetPositionX(float position);
    void SetPositionY(float position);
    void SetPositionZ(float position);
    void zrotate(float degrees);
    void MoveForward(float amount);
    void MoveZ(float amount);
    void setZRotation(float rotation);
    float* GetZRotation();
private:
    //going to have this stored in a matrix at some point
    float position[3];
    float zrotation;
};

#endif