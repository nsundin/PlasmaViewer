#include "prpengine.h"


template <class T>
T *prpengine::getModifierOfType(plSceneObject* sObj, T*(type)(plCreatable *pCre)) {
    for(size_t i = 0; i < sObj->getNumModifiers(); i++) {
        T *ret = type(sObj->getModifier(i)->getObj());
        if(ret != 0) return ret;
    }
    return 0;
}

void prpengine::NextSpawnPoint(camera &cam) {
    if(!SpawnPoints.empty())
        SetSpawnPoint((curSpawnPoint + 1) % SpawnPoints.getSize(), cam);
}

void prpengine::PrevSpawnPoint(camera &cam) {
    if(!SpawnPoints.empty()) {
        if(curSpawnPoint)
            SetSpawnPoint((curSpawnPoint - 1), cam);
        else
            SetSpawnPoint(SpawnPoints.getSize() - 1, cam);
    }
}

void prpengine::SetSpawnPoint(int idx, camera &cam) {
    plSceneObject* linkinpoint = plSceneObject::Convert(SpawnPoints[idx]->getObj());
    if (linkinpoint->getCoordInterface().Exists()) {
        printf("Spawning to: %s\n", SpawnPoints[idx]->getName().cstr());
        plCoordinateInterface* coord = plCoordinateInterface::Convert(linkinpoint->getCoordInterface()->getObj());
        hsMatrix44 mat = coord->getLocalToWorld();
        cam.warp(mat(0,3),mat(1,3),mat(2,3)+5); //raise ourselves up a bit
        float angles = asin(mat(1,0));
        float anglec = acos(mat(0,0));
        if(mat(1,0)<0) anglec = -anglec;
        if(mat(0,0)<0) angles = 3.141592-angles;
        //printf("%f %f\n", anglec, angles);
        cam.angle = angles;
        cam.turn();
        curSpawnPoint = idx;
    }
}
bool prpengine::SetSpawnPoint(plString name, camera &cam) {
    for (size_t i = 0; i < SpawnPoints.getSize(); i++) {
        if ((const plString)"LinkInPointDefault" == SpawnPoints[i]->getName())
        {
            SetSpawnPoint(i, cam);
            return true;
        }
    }
    return false;
}

void prpengine::UpdateSpawnPoints() {
    for (size_t i = 0; i < AllLoadedSceneObjects.size(); i++) {
        if(getModifierOfType(plSceneObject::Convert(AllLoadedSceneObjects[i]->getObj()), plSpawnModifier::Convert))
            SpawnPoints.push(AllLoadedSceneObjects[i]);
    }
}

void prpengine::AttemptToSetPlayerToLinkPointDefault(camera &cam) {
    // let's get the linkinpointdefault (if we can)
    //printf("\n: LinkInPointDefault :\n");
    //printf("lookin'...\n");
    if(!SetSpawnPoint(plString("LinkInPointDefault"), cam)) {
        printf("Couldn't find a link-in point\n");
        printf("...Attempting to spawn to next spawn-point on list instead.\n");
        NextSpawnPoint(cam);
    }
}


void prpengine::AttemptToSetFniSettings(plString fnipath) {
    fni fniFile;
    if (fniFile.load(fnipath)) {
        printf("\n: FNI File :\n");
        glClearColor(fniFile.fClearColor[0], fniFile.fClearColor[1], fniFile.fClearColor[2], 1.0f);
        GLfloat fogcol[] = {fniFile.fDefColor[0], fniFile.fDefColor[1], fniFile.fDefColor[2], 1.0f};

        printf("%f, %f, %f\n",fniFile.fDefLinear[0],fniFile.fDefLinear[1],fniFile.fDefLinear[2]);
        glFogfv(GL_FOG_COLOR, fogcol);
        if (fniFile.fFogType == fni::kLinear) {
            glEnable(GL_FOG);
            glFogi(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, fniFile.fDefLinear[0]);
            glFogf(GL_FOG_END, fniFile.fDefLinear[1]);
            glFogf(GL_FOG_DENSITY, fniFile.fDefLinear[2]);
        }
        if (fniFile.fFogType == fni::kExp2) {
            glEnable(GL_FOG);
            glFogi(GL_FOG_MODE, GL_EXP2);
            glFogf(GL_FOG_START, 0.0f);
            glFogf(GL_FOG_END, fniFile.fDefExp2[0]);
            glFogf(GL_FOG_DENSITY, fniFile.fDefExp2[1]);
        }
    }
    else printf("\n No FNI File @ %s\n",fnipath.cstr());
}


int prpengine::loadHeadSpinMipmapTexture(plKey mipmapkey,int texname) {

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

void prpengine::LoadTextures(std::vector<plKey> Textures) {
    ///Warning, until a problem with the texture-class list is fixed do not run this more than once (for fear of mem-leak)
    if (gl_texlist != NULL)
        delete[] gl_texlist;

    gl_texlist = new GLuint[Textures.size()];
    glGenTextures(Textures.size(), gl_texlist);
    for (size_t i=0; i < Textures.size(); i++) {
        TextureObject* tex = new TextureObject;
        tex->key = Textures[i];
        if (loadHeadSpinMipmapTexture(Textures[i],gl_texlist[i])) {
            tex->textureInd = gl_texlist[i];
        }
        else {
            tex->textureInd = -1;
        }
        TextureList.push_back(tex);
    }
}

int prpengine::getTextureIDFromKey(plKey in_key) {
    for (size_t imageidx = 0; imageidx < TextureList.size(); imageidx++) {
        if (TextureList[imageidx]->key->getName() == in_key->getName()) {

            return TextureList[imageidx]->textureInd;
        }
    }
    return -1;
}

void prpengine::UnloadObjects(int sequenceprefix) {
    for (size_t i = 0; i < TextureList.size(); i++) {
        if (TextureList[i]->key->getLocation().getSeqPrefix() == sequenceprefix) {
            TextureList.erase(TextureList.begin()+i);
            i-=1;
        }
    }
    for (size_t i = 0; i < DrawableList.size(); i++) {
        if (DrawableList[i]->Owner->getLocation().getSeqPrefix() == sequenceprefix) {
            DrawableList.erase(DrawableList.begin()+i);
            i-=1;
        }
    }
    for (size_t i = 0; i < AllLoadedSceneObjects.size(); i++) {
        if (AllLoadedSceneObjects[i]->getLocation().getSeqPrefix() == sequenceprefix) {
            AllLoadedSceneObjects.erase(AllLoadedSceneObjects.begin()+i);
            i-=1;
        }
    }
}

    //if (rendermode == 0) {
    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //    glDisable(GL_TEXTURE_2D);
    //    glDisable(GL_BLEND);
    //    float amb[4] = {1.0f,1.0f,1.0f,1.0f};
    //    float dif[4] = {1.0f,1.0f,1.0f,1.0f};
    //    float spec[4] = {1.0f,1.0f,1.0f,1.0f};
    //    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
    //    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
    //    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    //    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, amb);

    //    //glLineWidth(0.1f);
    //    glBegin(GL_TRIANGLES);
    //    for (size_t j = 0; j < indices.getSize(); j++) {
    //        int indice = indices[j];
    //        hsVector3 pos = verts[indice].fPos;
    //        glColor4f(1.0f,1.0f,1.0f,1.0f);
    //        glVertex3f(pos.X,pos.Y ,pos.Z);
    //    }
    //    glEnd();
    //}
