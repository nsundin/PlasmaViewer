#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <math.h>
#include <SDL/SDL_opengl.h>

class Camera {
public:
	Camera();
	void warp(float x,float y,float z);
	void update();
	float camFOV;
	float angle;
	float getCamPos(int i);
	float getCamPosL(int i);
	void turn();
	void moveLocalY(float speed);
	void moveLocalZ(float speed);
	float cam[3];
private:
	float camL[3];
};

#endif

