#include "LinkManager.h"

void LinkManager::PrintProgress(float progresspercent) {
	int bar_width = 30;
	printf("\r[");
	for (size_t i=0; i<(int)(bar_width*progresspercent); i++) {
		printf("=");
	}
	for (size_t i=0; i<bar_width-(int)(bar_width*progresspercent); i++) {
		printf(" ");
	}
	printf("] %1.2f%%",progresspercent*100);
	if (1.0f <= progresspercent) {
		printf("\n");
	}
}

void LinkManager::LoadLocation(const plLocation &loc) {
    LoadTextures(rm->getKeys(loc, kMipmap));
    AddObjects(rm->getKeys(loc, kSceneObject));
    AppendClustersToDrawList(rm->getKeys(loc, kClusterGroup));
}

int LinkManager::Load(const char* filename) {
	plDebug::Init(plDebug::kDLNone);
    printf("Loading Scene... use arrow keys to move around and Z and X to move up and down\n\n");
	window->isLoadingScreen = true;
	printf("setLoadingScreenTrue\n");
    if (plString(filename).afterLast('.') == "age") {
        plAgeInfo* age = rm->ReadAge(filename, false);

        //read the pages... because of the progress bar we can't leave this up to the resMgr with ReadAge
        plString path = plString(filename).beforeLast(PATHSEP);
        if (path.len() > 0)
            path = path + PATHSEP;

        if (age->getNumPages() > 0) {
            plString file = plString::Format("%s_District_%s.prp", age->getAgeName().cstr(), age->getPage(0).fName.cstr());
            FILE* F = fopen((path + file).cstr(), "rb");
            if (F == NULL) {
                rm->setVer(pvEoa, true);
            } else {
                rm->setVer(pvPots, true);
                fclose(F);
            }
        }
		ProgressCallback old = rm->SetProgressFunc(&UpdateProgressBar);

        for (size_t i=0; i<age->getNumPages(); i++) {
			printf("\nLoading %s\n\n",age->getPageFilename(i, rm->getVer()).cstr());
            rm->ReadPage(path + age->getPageFilename(i, rm->getVer()));
        }
        for (size_t i=0; i<age->getNumCommonPages(rm->getVer()); i++) {
            rm->ReadPage(path + age->getCommonPageFilename(i, rm->getVer()));
        }
		printf("\n");
        rm->SetProgressFunc(old);
        //end of page-reading

        for (size_t i1 = 0; i1 < age->getNumPages(); i1++) {
            LoadLocation(age->getPageLoc(i1,rm->getVer()));
        }
        for (size_t i1 = 0; i1 < age->getNumCommonPages(rm->getVer()); i1++) {
            LoadLocation(age->getCommonPageLoc(i1,rm->getVer()));
        }
    }
    else if (plString(filename).afterLast('.') == "prp") {
        LoadLocation(rm->ReadPage(filename)->getLocation());
        plString base = plString(filename).beforeLast('_');
        try {
            plString texs = base + plString("_Textures.prp");
            LoadLocation(rm->ReadPage(texs)->getLocation());
        }
        catch(...) {}
    }
//    prp_engine->UpdateSpawnPoints();
	window->isLoadingScreen = false;
	printf("setLoadingScreenFalse\n");
    return 1;
}

void LinkManager::AppendClusterGroupToDrawList(plKey clustergroupkey) {
    DrawableObject* dObj = new DrawableObject;
	dObj->poolinstance = this;
    plClusterGroup* cluster = plClusterGroup::Convert(clustergroupkey->getObj());
    dObj->isCluster = true;
    dObj->ClusterGroup = cluster;
    dObj->renderlevel = cluster->getRenderLevel();
    pool->DrawableList.push_back(dObj);
}

void LinkManager::AddObject(plKey sobjectkey) {
    plSceneObject* obj = plSceneObject::Convert(sobjectkey->getObj());
	EngineObject* eObj = new EngineObject;
	eObj->PlasmaKey = sobjectkey;
	eObj->ObjectName = (char*)sobjectkey->getName().cstr();
	if (obj->getSimInterface().Exists()) {
        plSimulationInterface* sim = plSimulationInterface::Convert(obj->getSimInterface()->getObj());
		if (sim->getPhysical().Exists()) {
			plGenericPhysical* phys = plGenericPhysical::Convert(sim->getPhysical()->getObj());
			PhysicalObject* pobj = new PhysicalObject;
			pobj->ObjOwner = eObj;
			pobj->Owner = sobjectkey;
			pobj->physical = phys;
			pobj->isInODE = false;
			pool->AddPhysicalObject(pobj);
			eObj->PhysObject = pobj;
		}
	}
	plCoordinateInterface* coord = NULL;
	if (obj->getCoordInterface().Exists()) {
		coord = plCoordinateInterface::Convert(obj->getCoordInterface()->getObj());
		eObj->ObjectMatrix = coord->getLocalToWorld();
		eObj->usesMatrix = true;
	}
	else eObj->usesMatrix = false;

    if (obj->getDrawInterface().Exists()) {
        plDrawInterface* draw = plDrawInterface::Convert(obj->getDrawInterface()->getObj());
//        if(!draw) return;
//		else eObj->usesMatrix = false;
        for (size_t i1=0; i1<draw->getNumDrawables(); i1++) {
            if (draw->getDrawableKey(i1) == -1){
                continue;
            }
            DrawableObject* dObj = new DrawableObject;
			dObj->poolinstance = this;
			dObj->ObjOwner = eObj;
            dObj->DrawableKey = draw->getDrawableKey(i1);
            dObj->SpanKey = draw->getDrawable(i1);
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
            pool->DrawableList.push_back(dObj);
			eObj->Drawables.push_back(dObj);
        }
    }
	pool->EngineObjects.push_back(eObj);
}

void LinkManager::AppendClustersToDrawList(std::vector<plKey> clusters) {
    for (size_t i = 0; i < clusters.size(); i++) {
        if (clusters[i].Exists()) {
            AppendClusterGroupToDrawList(clusters[i]);
            printf("ADDED CLUSTER: %s\n",clusters[i]->getName().cstr());
        }
    }
}

void LinkManager::AddObjects(std::vector<plKey> SObjects) {
    for (size_t i=0; i < SObjects.size(); i++) {
        AddObject(SObjects[i]);
    }
}

void LinkManager::LoadTextures(std::vector<plKey> Textures) {
    for (size_t i=0; i < Textures.size(); i++) {
        TextureObject* tex = new TextureObject;
        tex->key = Textures[i];
		tex->textureInd = -1;
        pool->TextureList.push_back(tex);
    }
}


//void LinkManager::UnloadAge(const plString agename) {
//    if (!rm->FindAge(agename)) return;
//    plAgeInfo * age = rm->FindAge(agename);
//    prp_engine->UnloadObjects(age->getSeqPrefix());
//    prp_engine->UpdateList(false);
//    rm->UnloadAge(agename);
//    printf("%s unloaded. %i locations loaded\n",agename.cstr(),	rm->getLocations().size());
//}
//            float completed = ((float)i+1.0f)/(float)age->getNumPages();
//            drawLoading(completed, 0.0);
//        drawLoading(0.0, 0.0);
