#include "plglSpawnPtMgr.h"


plglSpawnPtMgr::plglSpawnPtMgr() { }

void plglSpawnPtMgr::init(plResManager* rm) {
    this->rm = rm;
}

template <class T>
T *plglSpawnPtMgr::getModifierOfType(plSceneObject* sObj, T*(type)(plCreatable *pCre)) {
    for(size_t i = 0; i < sObj->getNumModifiers(); i++) {
        T *ret = type(sObj->getModifier(i)->getObj());
        if(ret != 0) return ret;
    }
    return 0;
}
bool plglSpawnPtMgr::SceneObjectHasSpawnMod(plSceneObject* sObj) {
    for(size_t i = 0; i < sObj->getNumModifiers(); i++) {
        if (sObj->getModifier(i)->getType() == kSpawnModifier) {
            return true;
        }
    }
    return false;
}

void plglSpawnPtMgr::NextSpawnPoint(plglCamera2 &cam) {
    if(!SpawnPoints.empty())
        SetSpawnPoint((curSpawnPoint + 1) % SpawnPoints.getSize(), cam);
}

void plglSpawnPtMgr::PrevSpawnPoint(plglCamera2 &cam) {
    if(!SpawnPoints.empty()) {
        if(curSpawnPoint)
            SetSpawnPoint((curSpawnPoint - 1), cam);
        else
            SetSpawnPoint(SpawnPoints.getSize() - 1, cam);
    }
}

void plglSpawnPtMgr::SetSpawnPoint(int idx, plglCamera2 &cam) {
    plSceneObject* linkinpoint = plSceneObject::Convert(SpawnPoints[idx]->getObj());
    if (linkinpoint->getCoordInterface().Exists()) {
        printf("Spawning to: %s\n", SpawnPoints[idx]->getName().cstr());
        plCoordinateInterface* coord = plCoordinateInterface::Convert(linkinpoint->getCoordInterface()->getObj());
        hsMatrix44 mat = coord->getLocalToWorld();
        cam.warp(mat(0,3),mat(1,3),mat(2,3)+5);
        float angles = asin(mat(1,0));
        float anglec = acos(mat(0,0));
        if(mat(1,0)<0) anglec = -anglec;
        if(mat(0,0)<0) angles = 3.141592-angles;
        //printf("%f %f\n", anglec, angles);
        cam.turn(angles);
        curSpawnPoint = idx;
    }
}
bool plglSpawnPtMgr::SetSpawnPoint(plString name, plglCamera2 &cam) {
    for (size_t i = 0; i < SpawnPoints.getSize(); i++) {
        if ((const plString)"LinkInPointDefault" == SpawnPoints[i]->getName())
        {
            SetSpawnPoint(i, cam);
            return true;
        }
    }
    return false;
}

void plglSpawnPtMgr::UpdateSpawnPoints() {
    std::vector<plLocation> locs = rm->getLocations();
    for (size_t i=0; i < locs.size(); ++i) {
        std::vector<plKey> sos = rm->getKeys(locs[i], kSceneObject);
        for (size_t i2=0; i2 < sos.size(); ++i2) {
            if (SceneObjectHasSpawnMod(plSceneObject::Convert(sos[i2]->getObj()))) {
                printf("Adding Spawnpoint %s to list",sos[i2]->getName().cstr());
                SpawnPoints.push(sos[i2]);
            }
        }
    }
}


void plglSpawnPtMgr::AttemptToSetPlayerToLinkPointDefault(plglCamera2 &cam) {
    // let's get the linkinpointdefault (if we can)
    //printf("\n: LinkInPointDefault :\n");
    //printf("lookin'...\n");
    if(!SetSpawnPoint(plString("LinkInPointDefault"), cam)) {
        printf("Couldn't find a link-in point\n");
        printf("...Attempting to spawn to next spawn-point on list instead.\n");
        NextSpawnPoint(cam);
    }
}

