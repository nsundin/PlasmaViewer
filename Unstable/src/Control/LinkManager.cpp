#include "LinkManager.h"

void LinkManager::LoadLocation(const plLocation &loc) {
//    pool->LoadTextures(rm->getKeys(loc, kMipmap));
    pool->AppendSceneObjectsToDrawList(rm->getKeys(loc, kSceneObject));
    pool->AppendClustersToDrawList(rm->getKeys(loc, kClusterGroup));
	pool->AppendSceneObjectsToList(rm->getKeys(loc, kSceneObject));
}

int LinkManager::Load(const char* filename) {
    printf("Loading Scene... use arrow keys to move around and Z and X to move up and down\n\n");
    if (plString(filename).afterLast('.') == "age") {
        plAgeInfo* age = rm->ReadAge(filename, false);

        //read the pages... because of the progress bar we can't leave this up to the resMgr with ReadAge
        plString path = plString(filename).beforeLast(PATHSEP);
        if (path.len() > 0)
            path = path + PATHSEP;

        if (age->getNumPages() > 0) {
            plString file = plString::Format("%s_District_%s.prp",
                    age->getAgeName().cstr(),
                    age->getPage(0).fName.cstr());
            FILE* F = fopen((path + file).cstr(), "rb");
            if (F == NULL) {
                rm->setVer(pvEoa, true);
            } else {
                rm->setVer(pvPots, true);
                fclose(F);
            }
        }
        int num_total_pages = age->getNumPages();
//        drawLoading(0.0, 0.0);
//		ProgressCallback old = rm->SetProgressFunc(&drawSecondaryProgress);
        for (size_t i=0; i<age->getNumPages(); i++) {
            rm->ReadPage(path + age->getPageFilename(i, rm->getVer()));
            float completed = ((float)i+1.0f)/(float)num_total_pages;
//            drawLoading(completed, 0.0);
        }
        for (size_t i=0; i<age->getNumCommonPages(rm->getVer()); i++) {
            rm->ReadPage(path + age->getCommonPageFilename(i, rm->getVer()));
        }
//        rm->SetProgressFunc(old);
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
    return 1;
}

//void LinkManager::UnloadAge(const plString agename) {
//    if (!rm->FindAge(agename)) return;
//    plAgeInfo * age = rm->FindAge(agename);
//    prp_engine->UnloadObjects(age->getSeqPrefix());
//    prp_engine->UpdateList(false);
//    rm->UnloadAge(agename);
//    printf("%s unloaded. %i locations loaded\n",agename.cstr(),	rm->getLocations().size());
//}
