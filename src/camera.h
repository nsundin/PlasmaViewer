#ifndef __CAMERA_H__
#define __CAMERA_H__

#ifdef WIN32
#include <windows.h>
#endif

#include <math.h>
#include <GL/glu.h>

class camera {
public:
	camera();
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

