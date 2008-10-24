#include "prpengine.h"

float* getMatrixFrom_hsMatrix44(hsMatrix44 data) {
    static float mat[16];
    mat[ 0] = data(0,0);
    mat[ 1] = data(1,0);
    mat[ 2] = data(2,0);
    mat[ 3] = data(3,0);
    mat[ 4] = data(0,1);
    mat[ 5] = data(1,1);
    mat[ 6] = data(2,1);
    mat[ 7] = data(3,1);
    mat[ 8] = data(0,2);
    mat[ 9] = data(1,2);
    mat[10] = data(2,2);
    mat[11] = data(3,2);
    mat[12] = data(0,3);
    mat[13] = data(1,3);
    mat[14] = data(2,3);
    mat[15] = data(3,3);
    return mat;
}

template <class T>
T *prpengine::getModifierOfType(plSceneObject *sObj, T*(type)(plCreatable *pCre)) {
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
	if(!SpawnPoints.empty())
		if(curSpawnPoint)
			SetSpawnPoint((curSpawnPoint - 1), cam);
		else
			SetSpawnPoint(SpawnPoints.getSize() - 1, cam);
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
	for (int i = 0; i < SpawnPoints.getSize(); i++) {
        if (name == SpawnPoints[i]->getName())
		{
			SetSpawnPoint(i, cam);
			return true;
		}
	}
	return false;
}
void prpengine::AttemptToSetPlayerToLinkPointDefault(hsTArray<plKey> SObjects,camera &cam) {
    // let's get the linkinpointdefault (if we can)
    //printf("\n: LinkInPointDefault :\n");
    //printf("lookin'...\n");
	size_t i;
    for (i = 0; i < SObjects.getSize(); i++) {
		if(getModifierOfType(plSceneObject::Convert(SObjects[i]->getObj()), plSpawnModifier::Convert))
			SpawnPoints.push(SObjects[i]);
	}
	curSpawnPoint = 0;
 	if(!SetSpawnPoint(plString("LinkInPointDefault"), cam))
		printf("Couldn't find a link-in point\n");
}

void prpengine::AddSceneObjectToDrawableList(plKey sobjectkey) {
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
			dObj->vfm = 0;
			for(int i = 0; i < obj->getNumModifiers(); i++) {
				plViewFaceModifier * vfm = plViewFaceModifier::Convert(obj->getModifier(i)->getObj());
				if(vfm){
					dObj->vfm = vfm;
					break;
				}
			}
            DrawableList.push_back(dObj);
        }
        SortDrawableList();
    }
}


void prpengine::AppendAllObjectsToDrawList(hsTArray<plKey> SObjects) {
    for (size_t i=0; i < SObjects.getSize(); i++) {
        AddSceneObjectToDrawableList(SObjects[i]);
    }
}
void prpengine::AttemptToSetFniSettings(plString filename) {
    glEnable(GL_FOG);
    fni fniFile;
    plString fnipath = (filename.beforeLast('.')+plString(".fni"));
    if (fniFile.load(fnipath)) {
        printf("\n: FNI File :\n");
        glClearColor(fniFile.fClearColor[0], fniFile.fClearColor[1], fniFile.fClearColor[2], 1.0f);
        GLfloat fogcol[] = {fniFile.fDefColor[0], fniFile.fDefColor[1], fniFile.fDefColor[2], 1.0f};

        glFogfv(GL_FOG_COLOR, fogcol);
        if (fniFile.fFogType == fni::kLinear) {
            glFogi(GL_FOG_MODE, GL_LINEAR);
            glFogf(GL_FOG_START, fniFile.fDefLinear[0]);
            glFogf(GL_FOG_END, fniFile.fDefLinear[1]);
            glFogf(GL_FOG_DENSITY, fniFile.fDefLinear[2]);
        }
        if (fniFile.fFogType == fni::kExp2) {
            glFogi(GL_FOG_MODE, GL_EXP2);
            glFogf(GL_FOG_START, 0.0f);
            glFogf(GL_FOG_END, fniFile.fDefExp2[0]);
            glFogf(GL_FOG_DENSITY, fniFile.fDefExp2[1]);
        }
    }
}

int prpengine::RenderDrawable(DrawableObject* dObj, int rendermode, camera &cam) {
    plDrawableSpans* span = plDrawableSpans::Convert(dObj->SpanKey->getObj());
    plDISpanIndex di = span->getDIIndex(dObj->DrawableKey);
    if ((di.fFlags & plDISpanIndex::kMatrixOnly) != 0) {
        return 0;
    }
    if(dObj->draw->getProperties().get(0)) return 0;
    for (size_t idx=0; idx<di.fIndices.getSize(); idx++) {
        plIcicle* ice = (plIcicle*)span->getSpan(di.fIndices[idx]);
        hsTArray<plGBufferVertex> verts = span->getVerts(ice);
        hsTArray<unsigned short> indices = span->getIndices(ice);
        
        //searching for a honest matrix
        hsMatrix44 objtransform;
//        if (dObj->hasCI) {
//            objtransform = dObj->CIMat;
//        }
//        else {
//        objtransform = ice->getLocalToWorld();
//        }

        if (rendermode == 0) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
            float amb[4] = {1.0f,1.0f,1.0f,1.0f};
            float dif[4] = {1.0f,1.0f,1.0f,1.0f};
            float spec[4] = {1.0f,1.0f,1.0f,1.0f};
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, amb);
            
            //glLineWidth(0.1f);
            glBegin(GL_TRIANGLES);
            for (size_t j = 0; j < indices.getSize(); j++) {
                int indice = indices[j];
                hsVector3 pos = verts[indice].fPos;// * objtransform;
                glColor4f(1.0f,1.0f,1.0f,1.0f);
                glVertex3f(pos.X,pos.Y ,pos.Z);
            }
            glEnd();
        }

        if (rendermode == 1) {
            plKey materialkey = span->getMaterial(ice->getMaterialIdx());
            if (!materialkey.isLoaded()) {
                return 0;
            }
            hsGMaterial* material = hsGMaterial::Convert(materialkey->getObj());
            for (size_t layeridx = 0; layeridx < material->getNumLayers(); layeridx++) {
                plKey layerkey = material->getLayer(layeridx);
                plLayerInterface* layer = plLayerInterface::Convert(layerkey->getObj());
                size_t uvSrc = layer->getUVWSrc() & 0xFFFF;
                if (!layer->getTexture() == NULL) {
                    if (layer->getTexture().isLoaded()) {
                        int texID = getTextureIDFromKey(layer->getTexture());
                        if (texID != -1) {
                            glEnable(GL_TEXTURE_2D);
                            glBindTexture(GL_TEXTURE_2D, texID);
                        }
                        else {
                            printf("Bad texture: %s\n", layer->getTexture()->getName().cstr());
                            glDisable(GL_TEXTURE_2D);
                        }
                    }
                    else {
                        printf("Texture not loaded: %s\n", layer->getTexture()->getName().cstr()); 
                        glDisable(GL_TEXTURE_2D);
                    }
                }
                else {
                    glDisable(GL_TEXTURE_2D);
                }
                bool is2Sided = (layer->getState().fMiscFlags & hsGMatState::kMiscTwoSided) != 0;
                if (is2Sided) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glDisable(GL_CULL_FACE);
                }
                else {
                    glPolygonMode(GL_FRONT, GL_FILL);
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                }
                float amb[4] = { layer->getAmbient().r, layer->getAmbient().g,
								 layer->getAmbient().b, layer->getAmbient().a };
                float dif[4] = { layer->getRuntime().r, layer->getRuntime().g,
                                 layer->getRuntime().b, layer->getRuntime().a };
                float spec[4] = { layer->getSpecular().r, layer->getSpecular().g,
                                  layer->getSpecular().b, layer->getSpecular().a };
                glMaterialfv(is2Sided ? GL_FRONT : GL_FRONT_AND_BACK, GL_AMBIENT, amb);
                glMaterialfv(is2Sided ? GL_FRONT : GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
                glMaterialfv(is2Sided ? GL_FRONT : GL_FRONT_AND_BACK, GL_SPECULAR, spec);
                if (layer->getState().fShadeFlags & hsGMatState::kShadeEmissive)
                    glMaterialfv(is2Sided ? GL_FRONT : GL_FRONT_AND_BACK, GL_EMISSION, amb);
                //glPixelTransferf(GL_ALPHA_SCALE, layer->getOpacity());
                
                glDisable(GL_BLEND);
                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
                if ((layer->getState().fBlendFlags & hsGMatState::kBlendAlpha) != 0) {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                }
                if ((layer->getState().fBlendFlags & hsGMatState::kBlendAdd) != 0) {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_ONE, GL_ONE);
                    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                }
                int useColor = GL_TRUE, useAlpha = GL_TRUE;
                if ((layer->getState().fBlendFlags & hsGMatState::kBlendNoTexColor) != 0) {
                    useColor = GL_FALSE;
                }
                if ((layer->getState().fBlendFlags & hsGMatState::kBlendNoTexAlpha) != 0) {
                    useAlpha = GL_FALSE;
                }
                glColorMask(useColor, useColor, useColor, useAlpha);

				//now our mesh
                glBegin(GL_TRIANGLES);
                for (size_t j = 0; j < indices.getSize(); j++) {
                    int indice = indices[j];
                    hsVector3 pos;
                    if (!dObj->hasCI) {
                        pos = verts[indice].fPos;// * objtransform;
                    }
                    else {
                        pos = verts[indice].fPos;
                    }
                    hsVector3 uvw = verts[indice].fUVWs[uvSrc] * layer->getTransform();
                    glTexCoord2f(uvw.X,uvw.Y);
                
                    hsColor32 col = verts[indice].fColor;
					glColor4ub(col.r,col.g,col.b,col.a==1?255:col.a);
                    glNormal3f(verts[indice].fNormal.X,verts[indice].fNormal.Y,verts[indice].fNormal.Z);
                    glVertex3f(pos.X,pos.Y ,pos.Z);
                }
                glEnd();
            }
        }
    }
    return 1;
}

void prpengine::l3dBillboardSphericalBegin(float *cam, float *worldPos) {

//	glPushMatrix();
//	glLoadIdentity();
	float lookAt[3]={0,-1,0},objToCamProj[3],objToCam[3],upAux[3],angleCosine;

// objToCamProj is the vector in world coordinates from the local origin to the camera
// projected in the XZ plane
	objToCamProj[0] = cam[0] - worldPos[0] ;
	objToCamProj[1] = cam[1] - worldPos[1] ;
	objToCamProj[2] = 0 ;

// normalize both vectors to get the cosine directly afterwards
	mathsNormalize(objToCamProj);

// easy fix to determine wether the angle is negative or positive
// for positive angles upAux will be a vector pointing in the 
// positive y direction, otherwise upAux will point downwards
// effectively reversing the rotation.

	mathsCrossProduct(upAux,lookAt,objToCamProj);

// compute the angle
	angleCosine = mathsInnerProduct(lookAt,objToCamProj);

// perform the rotation. The if statement is used for stability reasons
// if the lookAt and v vectors are too close together then |aux| could
// be bigger than 1 due to lack of precision
	if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
		glRotatef(acos(angleCosine)*180/3.14,upAux[0], upAux[2], upAux[1]);	


// The second part tilts the object so that it faces the camera

// objToCam is the vector in world coordinates from the local origin to the camera
	objToCam[0] = cam[0] - worldPos[0] ;
	objToCam[1] = cam[1] - worldPos[1] ;
	objToCam[2] = cam[2] - worldPos[2] ;

// Normalize to get the cosine afterwards
	mathsNormalize(objToCam);

// Compute the angle between v and v2, i.e. compute the
// required angle for the lookup vector
	angleCosine = mathsInnerProduct(objToCamProj,objToCam);


// Tilt the object. The test is done to prevent instability when objToCam and objToCamProj have a very small
// angle between them
	if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
		if (objToCam[2] < 0)
			glRotatef(acos(angleCosine)*180/3.14,-1,0,0);	
		else
			glRotatef(acos(angleCosine)*180/3.14,1,0,0);	

}

void prpengine::UpdateList(hsTArray<plKey> SObjects, bool wireframe, bool firstTime, camera &cam) {
    printf("Renderlist Update\n");
	int count = 0;
	if(firstTime || gl_rendercount == 0) {
		if(!gl_rendercount)
			glDeleteLists(gl_renderlist, gl_rendercount);
		gl_rendercount = SObjects.getSize();
		gl_renderlist = glGenLists(gl_rendercount);
		for (size_t i=0; i < DrawableList.size(); i++) {
			DrawableList[i]->RenderIndex =  i;
			glNewList(gl_renderlist+i, GL_COMPILE);
			RenderDrawable(DrawableList[i],wireframe?0:1,cam);
			glEndList();
			count++;
		}
	}
	else {
		for (size_t i=0; i < DrawableList.size(); i++) {
			if(DrawableList[i]->vfm && DrawableList[i]->vfm->getFlag(plViewFaceModifier::kFaceCam)) {
				glDeleteLists(gl_renderlist+DrawableList[i]->RenderIndex, 1);
				glNewList(gl_renderlist+DrawableList[i]->RenderIndex, GL_COMPILE);
				RenderDrawable(DrawableList[i],wireframe?0:1,cam);
				glEndList();
				count++;
			}
		}
	}
	printf("Regenerated %d items\n", count);
}

bool SortDrawables(DrawableObject* lhs, DrawableObject* rhs) {
    if(lhs->renderlevel < rhs->renderlevel) return true;
    else if(lhs->renderlevel > rhs->renderlevel) return false;
    else if(lhs->spanflags < rhs->spanflags) return true;
    else if(lhs->spanflags > rhs->spanflags) return false;
	else
		if(lhs->hasCI && rhs->hasCI) {
		float a1 = lhs->CIMat(0,3) - cam.getCamPos(0); 
		float b1 = lhs->CIMat(1,3) - cam.getCamPos(1); 
		float c1 = lhs->CIMat(2,3) - cam.getCamPos(2); 
		float a2 = rhs->CIMat(0,3) - cam.getCamPos(0); 
		float b2 = rhs->CIMat(1,3) - cam.getCamPos(1); 
		float c2 = rhs->CIMat(2,3) - cam.getCamPos(2);
		return (a1*a1+b1*b1+c1*c1) > (a2*a2+b2*b2+c2*c2);
	}
	return false;
}

void prpengine::SortDrawableList() {
	std::stable_sort(DrawableList.begin(), DrawableList.end(), &SortDrawables);
}
void prpengine::PrintObjects() {
    for (size_t i=0; i < DrawableList.size(); i++) {
		DrawableObject *dObj = DrawableList[i];
		printf("%d: %s\n", i, dObj->Owner->getName().cstr());
	}
}
void prpengine::draw(camera &cam) {
	SortDrawableList();
    for (size_t i=0; i < DrawableList.size(); i++) {
		DrawableObject *dObj = DrawableList[i];
	//	printf("%d: %s\n", i, dObj->Owner->getName().cstr());
        glPushMatrix();
        if (dObj->hasCI) {
			glMultMatrixf(getMatrixFrom_hsMatrix44(DrawableList[i]->CIMat));
        }
		if(dObj->vfm) {
			if(dObj->vfm->getFlag(plViewFaceModifier::kFaceCam)) {
				if(dObj->hasCI) {
					float camV[3], objV[3];
					for(int i = 0; i < 3; i++) {
						camV[i] = cam.getCamPos(i);
						objV[i] = dObj->CIMat(i,3);
					}
					l3dBillboardSphericalBegin(camV, objV);
				}
			}
		}
        glCallList(gl_renderlist+dObj->RenderIndex);
		//RenderDrawable(dObj,1,cam);
        glPopMatrix();
    }
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
//		printf("Creating texture %d: w%d, h%d, l%d, t%i\n", texname, mipmapimage->getWidth(), mipmapimage->getHeight(), mipmapimage->getNumLevels(), 2* (DXCompressionType&15)-1);
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

void prpengine::LoadAllTextures(hsTArray<plKey> Textures) {
    ///Warning, until a problem with the texture-class list is fixed do not run this more than once (for fear of mem-leak)
    if (gl_texlist != NULL)
        delete[] gl_texlist;


    gl_texlist = new GLuint[Textures.getSize()];
    glGenTextures(Textures.getSize(), gl_texlist);
    for (size_t i=0; i < Textures.getSize(); i++) {
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
