#ifndef __PHYSICALOBJECT_H__
#define __PHYSICALOBJECT_H__

#include <stdlib.h>
#include <ode/ode.h>
#include "PRP/Physics/plGenericPhysical.h"
#include "Math/hsMatrix44.h"
#include "PRP/KeyedObject/plKey.h"
#include "EngineObject.h"

class PhysicalObject {
public:
	bool isInODE;
	dBodyID body;
	dGeomID Geom;
	dMass mass;
	EngineObject* ObjOwner;
	plGenericPhysical* physical;
	plKey Owner;
};

#endif
