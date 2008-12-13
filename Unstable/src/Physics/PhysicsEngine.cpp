#include "PhysicsEngine.h"

#define MAX_CONTACTS 10

void PhysicsEngine::Unload() {
	dJointGroupDestroy(contactGroup);
    dSpaceDestroy(space);

	dWorldDestroy(world);
	dCloseODE();
}

void PhysicsEngine::initWorldAndEngine() {
	NumPhysicalsLoaded = 0;
	dInitODE();
	world = dWorldCreate();
	dWorldSetGravity(world, 0.0, 0.0, -4.0);
	space = dSimpleSpaceCreate(0);
}

void PhysicsEngine::LoadAllObjectsIntoCalc(std::vector<PhysicalObject*> objs) {
	for (size_t i = 0; i < objs.size(); ++i) {
		addObject(objs[i]);
	}
}

//void PhysicsEngine::getGeom() {
//
//	if (obj->physical->getBoundsType() == plSimDefs::kProxyBounds) {
//		printf(":Proxy:");
//		dTriMeshDataID triMesh = dGeomTriMeshDataCreate();
//		size_t numVerts = obj->physical->getNumVerts();
//		dVector3* triVerts;
//		triVerts = new dVector3[numVerts];
//		for (size_t i = 0; i < numVerts; ++i) {
//			triVerts[i][0] = obj->physical->getVert(i).X;
//			triVerts[i][1] = obj->physical->getVert(i).Y;
//			triVerts[i][2] = obj->physical->getVert(i).Z;
//		}
//		size_t numInds = obj->physical->getNumIndices();
//		unsigned int* triInd;
//		triInd = new unsigned int[numInds];
//		for (size_t i = 0; i < numInds; ++i) {
//			triInd[i] = obj->physical->getIndex(i);
//		}
//
//		dGeomTriMeshDataBuildSimple(triMesh, (const dReal *)triVerts, numVerts, (const dTriIndex *)triInd, numInds);
//		obj->Geom = dCreateTriMesh(space, triMesh, NULL, NULL, NULL);
//	}
//	else if (obj->physical->getBoundsType() == plSimDefs::kSphereBounds) {
//		printf(":Sphere:");
//		obj->Geom = dCreateSphere(space, (dReal)obj->physical->getRadius());
//	}
//}
//	dBodySetPosition(obj->body,(dReal)obj->ObjOwner->ObjectMatrix(0,3),(dReal)obj->ObjOwner->ObjectMatrix(1,3),(dReal)obj->ObjOwner->ObjectMatrix(2,3));
//	dMassSetSphereTotal(&obj->mass, (dReal)obj->physical->getMass(), 1.0);
	//if (obj->Geom) {
	//	printf("settingGeom");
	//	dGeomSetData(obj->Geom, (void*)obj->physical->getKey()->getName().cstr());
	//	dGeomSetBody(obj->Geom, obj->body);
	//}


void PhysicsEngine::addObject(PhysicalObject* obj) {
	printf("adding %s to Phys Sim... ",obj->ObjOwner->ObjectName);

	obj->body = dBodyCreate(world);
	if (obj->physical->getMass() > 0) {
		dMassSetSphereTotal(&obj->mass, (dReal)obj->physical->getMass(), 1.0);
		dBodySetMass(obj->body, &obj->mass);
	}
	dBodySetPosition(obj->body,(dReal)obj->ObjOwner->ObjectMatrix(0,3),(dReal)obj->ObjOwner->ObjectMatrix(1,3),(dReal)obj->ObjOwner->ObjectMatrix(2,3));

	dBodySetLinearVel(obj->body, 0.0, 0.0, 0.0);
	obj->isInODE = true;
	NumPhysicalsLoaded +=1;
	printf("Done\n");
}

void PhysicsEngine::removeObject(PhysicalObject* obj) {
	dBodyDestroy(obj->body);
	dGeomDestroy(obj->Geom);
//	pool->DeletePhysicalObject(obj);
}

void PhysicsEngine::updateObjectMatrices() {
	for (size_t i = 0; i < pool->PhysicalObjects.size(); ++i) {
		if (!pool->PhysicalObjects[i]->isInODE) continue;
//		if (pool->PhysicalObjects[i]->ObjOwner->usesMatrix) printf("I USE A MAT!\n");
		const dReal* realP = dBodyGetPosition(pool->PhysicalObjects[i]->body);
		const dReal* realR = dBodyGetRotation(pool->PhysicalObjects[i]->body);
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(0,0) = realR[0];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(1,0) = realR[1];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(2,0) = realR[2];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(3,0) = realR[3];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(0,1) = realR[4];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(1,1) = realR[5];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(2,1) = realR[6];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(3,1) = realR[7];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(0,2) = realR[8];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(1,2) = realR[9];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(2,2) = realR[10];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(3,2) = realR[11];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(0,3) = realP[0];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(1,3) = realP[1];
		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(2,3) = realP[2];
//		pool->PhysicalObjects[i]->ObjOwner->ObjectMatrix(3,3) = 1.0;
	}
}

void PhysicsEngine::collisionHandler(void * cinstance, dGeomID o1, dGeomID o2) {
	PhysicsEngine* cl = (PhysicsEngine*)cinstance;
	dContactGeom contacts[MAX_CONTACTS];
	int collisions = dCollide(o1, o2, MAX_CONTACTS, contacts, sizeof(dContactGeom));
	for (int i = 0; i < collisions; ++i) {
		dGeomID g1 = contacts[i].g1,
				g2 = contacts[i].g2;

		if (g1 == g2)
			continue;

		float *pos = contacts[i].pos;
		cl->contactPoints[cl->contactPointsCount][0] = pos[0];
		cl->contactPoints[cl->contactPointsCount][1] = pos[1];
		cl->contactPoints[cl->contactPointsCount][2] = pos[2];
		cl->contactPointsCount++;

		char *o1Name = (char *)dGeomGetData(g1);
		char *o2Name = (char *)dGeomGetData(g2);
		const dReal *o1Pos = dGeomGetPosition(g1);
		const dReal *o2Pos = dGeomGetPosition(g2);
	}
}

void PhysicsEngine::nearCallback(void * cinstance, dGeomID o1, dGeomID o2) {
	PhysicsEngine* cl = (PhysicsEngine*)cinstance;
	dBodyID body1 = dGeomGetBody(o1);
	dBodyID body2 = dGeomGetBody(o2);

	dContact contact[MAX_CONTACTS];

	for (int i = 0; i < MAX_CONTACTS; i++) {
		contact[i].surface.mode = dContactBounce;
		contact[i].surface.bounce = 0.5;
		contact[i].surface.mu = 10.0;
	}

	int collisions = dCollide(o1, o2, MAX_CONTACTS, &contact[0].geom, sizeof(dContact));
	if (collisions) {
		for (int i = 0; i < collisions; ++i) {
			dJointID c = dJointCreateContact(cl->world, cl->contactGroup, contact + i);
			dJointAttach(c, body1, body2);
		}
	}
}

void PhysicsEngine::CalulateCollisions() {
	contactPointsCount = 0;
	dSpaceCollide(space, this, PhysicsEngine::collisionHandler);
	dSpaceCollide(space, this, PhysicsEngine::nearCallback);
}

void PhysicsEngine::ClearAndStepEngine(float stepsize) {
//	for (size_t i = 0; i < pool->PhysicalObjects.size(); ++i) {
//		if (!pool->PhysicalObjects[i]->isInODE) continue;
//		const dReal* realP = dBodyGetPosition(pool->PhysicalObjects[i]->body);
//		printf("Pos: %f, %f, %f\n",realP[0],realP[1],realP[2]);
//	}
	dWorldQuickStep(world, 0.05);
//	if (NumPhysicalsLoaded > 0)  dJointGroupEmpty(contactGroup);
}
