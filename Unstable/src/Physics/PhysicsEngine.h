#ifndef __PHYSICSENGINE_H__
#define __PHYSICSENGINE_H__

#include <ode/ode.h>
#include "../DataPool/DataPool.h"
#include <vector>

class PhysicsEngine {
public:
	DataPool* pool;
	void Unload();
	unsigned int NumPhysicalsLoaded;
	void LoadAllObjectsIntoCalc(std::vector<PhysicalObject*> objs);
	void initWorldAndEngine();
	void addObject(PhysicalObject* obj);
	void removeObject(PhysicalObject* obj);
	void updateObjectMatrices();
	static void collisionHandler(void * cinstance, dGeomID o1, dGeomID o2);
	static void nearCallback(void * cinstance, dGeomID o1, dGeomID o2);
	void CalulateCollisions();
	void ClearAndStepEngine(float stepsize);
	dSpaceID space;
	dWorldID world;
	dReal contactPoints[100][3];
	int contactPointsCount;
	dJointGroupID contactGroup;

};

#endif
