#include "DataPool.h"

void DataPool::AppendClusterGroupToDrawList(plKey clustergroupkey) {
    DrawableObject* dObj = new DrawableObject;

    plClusterGroup* cluster = plClusterGroup::Convert(clustergroupkey->getObj());
    dObj->isCluster = true;
    dObj->ClusterGroup = cluster;
    dObj->renderlevel = cluster->getRenderLevel();
    dObj->Owner = clustergroupkey;
    DrawableList.push_back(dObj);
//    SortDrawableList();
}

void DataPool::AppendSceneObjectToDrawList(plKey sobjectkey) {
    plSceneObject* obj = plSceneObject::Convert(sobjectkey->getObj());
    if (obj->getDrawInterface().Exists()) {
        plDrawInterface* draw = plDrawInterface::Convert(obj->getDrawInterface()->getObj());
        if(!draw) return;

        plCoordinateInterface* coord = NULL;
        if (obj->getCoordInterface().Exists()) {
            coord = plCoordinateInterface::Convert(obj->getCoordInterface()->getObj());
        }
        for (size_t i1=0; i1<draw->getNumDrawables(); i1++) {
            if (draw->getDrawableKey(i1) == -1){
                continue;
            }
            DrawableObject* dObj = new DrawableObject;
            if (coord == NULL) {
                dObj->hasCI = false;
            }
            else {
                dObj->hasCI = true;
                dObj->CIMat = coord->getLocalToWorld();
            }
            dObj->DrawableKey = draw->getDrawableKey(i1);
            dObj->SpanKey = draw->getDrawable(i1);
            dObj->Owner = sobjectkey;
            plDrawableSpans* span = plDrawableSpans::Convert(draw->getDrawable(i1)->getObj());
            dObj->renderlevel = span->getRenderLevel();
            dObj->spanflags = span->getProps();
            dObj->draw = draw;
            dObj->isCluster = false;
            dObj->vfm = 0;
            for(size_t i = 0; i < obj->getNumModifiers(); i++) {
                plViewFaceModifier * vfm = plViewFaceModifier::Convert(obj->getModifier(i)->getObj());
                if(vfm){
                    dObj->vfm = vfm;
                    break;
                }
            }
            dObj->isAnimPlaying = true;
            DrawableList.push_back(dObj);
        }
//        SortDrawableList();
    }
}

void DataPool::AppendClustersToDrawList(std::vector<plKey> clusters) {
    for (size_t i = 0; i < clusters.size(); i++) {
        if (clusters[i].Exists()) {
            AppendClusterGroupToDrawList(clusters[i]);
            printf("ADDED CLUSTER: %s\n",clusters[i]->getName().cstr());
        }
    }
}

void DataPool::AppendSceneObjectsToDrawList(std::vector<plKey> SObjects) {
    for (size_t i=0; i < SObjects.size(); i++) {
        AppendSceneObjectToDrawList(SObjects[i]);
    }
}

void DataPool::AppendSceneObjectsToList(std::vector<plKey> SObjects) {
    for (size_t i=0; i < SObjects.size(); i++) {
        AllLoadedSceneObjects.push_back(SObjects[i]);
    }
}

void DataPool::PrintObjects() {
    for (size_t i=0; i < DrawableList.size(); i++) {
        DrawableObject *dObj = DrawableList[i];
        printf("%d: %s\n", i, dObj->Owner->getName().cstr());
    }
}
TextureObject* DataPool::getTextureObject(size_t ind) {
	return TextureList[ind];
}

plKey DataPool::getSceneObject(size_t ind) {
	return AllLoadedSceneObjects[ind];
}

DrawableObject* DataPool::getDrawObject(size_t ind) {
	return DrawableList[ind];
}

size_t DataPool::getDrawObjectSize() {
	return DrawableList.size();
}

size_t DataPool::getTextureObjectSize() {
	return TextureList.size();
}

Player* DataPool::getPlayer(int PlayerID) {
    for (size_t i=0; i < Players.size(); i++) {
		if (Players[i]->PlayerID == PlayerID) {
			return Players[i];
		}
	}
}

Camera* DataPool::getCamera(int ind) {
	return Cameras[ind];
}

Camera* DataPool::createCamera() {
	Camera* cam = new Camera;
	Cameras.push_back(cam);
	return cam;
}

//bool SortDrawables(DrawableObject* lhs, DrawableObject* rhs) {
//    if(lhs->renderlevel < rhs->renderlevel) return true;
//    else if(lhs->renderlevel > rhs->renderlevel) return false;
//    else if(lhs->spanflags < rhs->spanflags) return true;
//    else if(lhs->spanflags > rhs->spanflags) return false;
//    else
//        if(lhs->hasCI && rhs->hasCI) {
//        float a1 = lhs->CIMat(0,3) - cam.getCamPos(0);
//        float b1 = lhs->CIMat(1,3) - cam.getCamPos(1);
//        float c1 = lhs->CIMat(2,3) - cam.getCamPos(2);
//        float a2 = rhs->CIMat(0,3) - cam.getCamPos(0);
//        float b2 = rhs->CIMat(1,3) - cam.getCamPos(1);
//        float c2 = rhs->CIMat(2,3) - cam.getCamPos(2);
//        return (a1*a1+b1*b1+c1*c1) > (a2*a2+b2*b2+c2*c2);
//    }
//    return false;
//}
//
//void DataPool::SortDrawableList() {
//    std::stable_sort(DrawableList.begin(), DrawableList.end(), &SortDrawables);
//}
