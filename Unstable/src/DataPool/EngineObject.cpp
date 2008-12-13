#include "EngineObject.h"

float* EngineObject::getPosition() {
	float loc[3];
	if (!usesMatrix) {
		loc[0] = 0.0f;
		loc[1] = 0.0f;
		loc[2] = 0.0f;
	}
	else {
		loc[0] = ObjectMatrix(0,3);
		loc[1] = ObjectMatrix(1,3);
		loc[2] = ObjectMatrix(2,3);
	}
	return (float*)loc;
}
