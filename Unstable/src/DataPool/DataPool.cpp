#include "DataPool.h"

bool SortDrawables(DrawableObject* lhs, DrawableObject* rhs) {
    if(lhs->renderlevel < rhs->renderlevel) return true;
    else if(lhs->renderlevel > rhs->renderlevel) return false;
    else if(lhs->spanflags < rhs->spanflags) return true;
    else if(lhs->spanflags > rhs->spanflags) return false;
    else if(lhs->ObjOwner->usesMatrix && rhs->ObjOwner->usesMatrix) {
		DataPool* poolinst = (DataPool*)lhs->poolinstance;
        float a1 = lhs->ObjOwner->ObjectMatrix(0,3) - poolinst->getCurrentCamera()->getCamPos(0);
        float b1 = lhs->ObjOwner->ObjectMatrix(1,3) - poolinst->getCurrentCamera()->getCamPos(1);
        float c1 = lhs->ObjOwner->ObjectMatrix(2,3) - poolinst->getCurrentCamera()->getCamPos(2);
        float a2 = lhs->ObjOwner->ObjectMatrix(0,3) - poolinst->getCurrentCamera()->getCamPos(0);
        float b2 = lhs->ObjOwner->ObjectMatrix(1,3) - poolinst->getCurrentCamera()->getCamPos(1);
        float c2 = lhs->ObjOwner->ObjectMatrix(2,3) - poolinst->getCurrentCamera()->getCamPos(2);
        return (a1*a1+b1*b1+c1*c1) > (a2*a2+b2*b2+c2*c2);
    }
    return false;
}

hsMatrix44 DataPool::getLinkInPointMatrix() {
	for (size_t i = 0; i < EngineObjects.size(); ++i) {
		if (EngineObjects[i]->ObjectName == "LinkInPointDefault" && EngineObjects[i]->usesMatrix) 
			return EngineObjects[i]->ObjectMatrix;
	}
	for (size_t i = 0; i < EngineObjects.size(); ++i) {
		plSceneObject* so = plSceneObject::Convert(EngineObjects[i]->PlasmaKey->getObj());
		for (size_t modi = 0; modi < so->getNumModifiers(); ++modi) {
			if (so->getModifier(modi)->getType() == kSpawnModifier) {
				return EngineObjects[i]->ObjectMatrix;
			}
		}
	}
	return hsMatrix44::Identity();
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

void DataPool::PrintObjects() {
//    for (size_t i=0; i < DrawableList.size(); i++) {
//        DrawableObject *dObj = DrawableList[i];
//        printf("%d: %s\n", i, dObj->PlasmaOwnerKey->getName().cstr());
//    }
}
TextureObject* DataPool::getTextureObject(size_t ind) {
	return TextureList[ind];
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

void DataPool::AddPhysicalObject(PhysicalObject* obj) {
	PhysicalObjects.push_back(obj);
}

void DataPool::DeletePhysicalObject(PhysicalObject* obj) {
	for (size_t i=0; i < PhysicalObjects.size(); i++) {
		if (PhysicalObjects[i] == obj) {
			delete[] PhysicalObjects[i];
			PhysicalObjects.erase(PhysicalObjects.begin()+i);
			return;
		}
	}
}
