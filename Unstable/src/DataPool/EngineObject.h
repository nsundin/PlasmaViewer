#ifndef __ENGINEOBJECT_H__
#define __ENGINEOBJECT_H__

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include "PRP/KeyedObject/plKey.h"
#include "Math/hsMatrix44.h"

class EngineObject {
public:
	float* getPosition();
	plKey PlasmaKey;
	char* ObjectName;
	hsMatrix44 ObjectMatrix;
	bool usesMatrix;
    std::vector<void*> Drawables;
	void* PhysObject;
};

#endif
