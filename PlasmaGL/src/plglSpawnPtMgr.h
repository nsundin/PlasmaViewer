#ifndef PLGLSPAWNPTMGR_H
#define PLGLSPAWNPTMGR_H

#include "PRP/Object/plSceneObject.h"
#include "PRP/Object/plCoordinateInterface.h"
#include "PRP/Modifier/plSpawnModifier.h"
#include "ResManager/plResManager.h"
#include "Math/hsMatrix44.h"

#include "Util/plString.h"
#include "Util/hsTArray.hpp"

#include "plglCamera2.h"
#include <math.h>

class plglSpawnPtMgr {
public:
    plglSpawnPtMgr();
    void init(plResManager* rm);
    void AttemptToSetPlayerToLinkPointDefault(plglCamera2 &cam);
    void UpdateSpawnPoints();
    void NextSpawnPoint(plglCamera2 &cam);
    void PrevSpawnPoint(plglCamera2 &cam);
    bool SetSpawnPoint(plString name, plglCamera2 &cam);
private:
    plResManager* rm;
    template <class T>
    T *getModifierOfType(plSceneObject *sObj, T*(type)(plCreatable *pCre));
    void SetSpawnPoint(int idxc, plglCamera2 &cam);
    bool SceneObjectHasSpawnMod(plSceneObject* sObj);
    hsTArray<plKey> SpawnPoints;
    int curSpawnPoint;
};

#endif
