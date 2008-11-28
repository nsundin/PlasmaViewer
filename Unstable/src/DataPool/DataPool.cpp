#include "DataPool.h"

bool SortDrawables(DrawableObject* lhs, DrawableObject* rhs) {
    if(lhs->renderlevel < rhs->renderlevel) return true;
    else if(lhs->renderlevel > rhs->renderlevel) return false;
    else if(lhs->spanflags < rhs->spanflags) return true;
    else if(lhs->spanflags > rhs->spanflags) return false;
    else if(lhs->hasCI && rhs->hasCI) {
		DataPool* poolinst = (DataPool*)lhs->poolinstance;
        float a1 = lhs->CIMat(0,3) - poolinst->getCurrentCamera()->getCamPos(0);
        float b1 = lhs->CIMat(1,3) - poolinst->getCurrentCamera()->getCamPos(1);
        float c1 = lhs->CIMat(2,3) - poolinst->getCurrentCamera()->getCamPos(2);
        float a2 = rhs->CIMat(0,3) - poolinst->getCurrentCamera()->getCamPos(0);
        float b2 = rhs->CIMat(1,3) - poolinst->getCurrentCamera()->getCamPos(1);
        float c2 = rhs->CIMat(2,3) - poolinst->getCurrentCamera()->getCamPos(2);
        return (a1*a1+b1*b1+c1*c1) > (a2*a2+b2*b2+c2*c2);
    }
    return false;
}

Camera* DataPool::getCurrentCamera() {
	return CurrentCamera;
}
void DataPool::SetCurrentCamera(Camera* cam) {
	CurrentCamera = cam;
}

void DataPool::SortDrawableList() {
    std::stable_sort(DrawableList.begin(), DrawableList.end(), &SortDrawables);
}

void DataPool::AppendClusterGroupToDrawList(plKey clustergroupkey) {
    DrawableObject* dObj = new DrawableObject;
	dObj->poolinstance = this;
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
			dObj->poolinstance = this;
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

int DataPool::loadHeadSpinMipmapTexture(plKey mipmapkey,int texname) {

PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = NULL;
#ifdef WIN32
    glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");
#else
    glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)glXGetProcAddress((const GLubyte*)"glCompressedTexImage2DARB");
#endif

    plMipmap* mipmapimage = plMipmap::Convert(mipmapkey->getObj());
    if(mipmapimage->getImageData()==0){
        printf("No image data for: %s\n", mipmapkey->getName().cstr());
    }
    if (mipmapimage->getCompressionType() == plBitmap::kDirectXCompression){
        unsigned int DXCompressionType = 0;
        if (mipmapimage->getDXCompression() == plBitmap::kDXT1)
            DXCompressionType = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        else if (mipmapimage->getDXCompression() == plBitmap::kDXT3)
            DXCompressionType = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        else if (mipmapimage->getDXCompression() == plBitmap::kDXT5)
            DXCompressionType = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

        else return 0;
        glBindTexture(GL_TEXTURE_2D, texname); //that thar is the ID
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//      printf("Creating texture %d: w%d, h%d, l%d, t%i\n", texname, mipmapimage->getWidth(), mipmapimage->getHeight(), mipmapimage->getNumLevels(), 2* (DXCompressionType&15)-1);
        for(unsigned int il = 0; il < mipmapimage->getNumLevels(); ++il) {
            glCompressedTexImage2DARB(GL_TEXTURE_2D, il, DXCompressionType, mipmapimage->getLevelWidth(il), mipmapimage->getLevelHeight(il), 0, mipmapimage->getLevelSize(il),(const unsigned long *)mipmapimage->getLevelData(il));
        }
        return 1;
    }
    /*else if (mipmapimage->getCompressionType() == plBitmap::kJPEGCompression) {
        size_t size = mipmapimage->GetUncompressedSize();
        unsigned char* jpgbuffer = new unsigned char[size];
        try {
            mipmapimage->DecompressImage(jpgbuffer, size);
            glBindTexture(GL_TEXTURE_2D, texname);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mipmapimage->getLevelWidth(0), mipmapimage->getLevelHeight(0), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, jpgbuffer);
        } catch (hsException& e) {
            delete[] jpgbuffer;
            return 0;
        }
        delete[] jpgbuffer;
    } */else {
        for (unsigned int il = 0; il < mipmapimage->getNumLevels(); ++il) {
            glBindTexture(GL_TEXTURE_2D, texname);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, il, GL_RGBA, mipmapimage->getLevelWidth(il), mipmapimage->getLevelHeight(il), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapimage->getLevelData(il));
        }
        return 1;
    }
        return 0;
}

void DataPool::LoadTextures(std::vector<plKey> Textures) {
    for (size_t i=0; i < Textures.size(); i++) {
        TextureObject* tex = new TextureObject;
        tex->key = Textures[i];
		tex->textureInd = -1;
        TextureList.push_back(tex);
    }
}

void DataPool::LoadTexturesToGL() {
    if (gl_texlist != NULL)
        delete[] gl_texlist;

    gl_texlist = new GLuint[TextureList.size()];
    glGenTextures(TextureList.size(), gl_texlist);
    for (size_t i=0; i < TextureList.size(); i++) {
        if (loadHeadSpinMipmapTexture(TextureList[i]->key, gl_texlist[i])) {
            TextureList[i]->textureInd = gl_texlist[i];
        }
        else {
            TextureList[i]->textureInd = -1;
        }
    }
}

unsigned int DataPool::RegisterMessage(EngineMessage* msg) {
	if (EngineMsgs.size() > 1000) return kErrorMsgNumberLimit;
	EngineMsgs.push_back(msg);
	return kAddedMsg;
}

void DataPool::DeleteMessage(EngineMessage* msg) {
	for (size_t i=0; i < EngineMsgs.size(); i++) {
		if (EngineMsgs[i] == msg) {
			delete[] EngineMsgs[i];
			EngineMsgs.erase(EngineMsgs.begin()+i);
			return;
		}
	}
}

void DataPool::DeleteMessageByInd(int i) {
	delete EngineMsgs[i];
	EngineMsgs.erase(EngineMsgs.begin()+i);
}

EngineMessage* DataPool::getMessage(int i) {
	return EngineMsgs[i];
}

size_t DataPool::getNumMsgs() {
	return EngineMsgs.size();
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
