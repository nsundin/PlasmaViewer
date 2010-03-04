#ifndef __PLGLCAMERA2_H__
#define __PLGLCAMERA2_H__

#include <math.h>
//#include <SDL_opengl.h>

class plglCamera2 {
public:
    plglCamera2();
    void warp(float x,float y,float z);
    void update();
    float camFOV;
    float getCamPos(int i);
    float getCamPosL(int i);
    void turn(float angle);
    void moveLocalY(float speed);
    void moveLocalZ(float speed);
    float cam[3];
private:
    float camL[3];
    float angle;
};

#endif

