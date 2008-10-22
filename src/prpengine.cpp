#include "prpengine.h"


void prpengine::AttemptToSetPlayerToLinkPointDefault(hsTArray<plKey> SObjects,camera &cam) {
    // let's get the linkinpointdefault (if we can)
    printf("\n: LinkInPointDefault :\n");
    printf("lookin'...\n");
    for (size_t i = 0; i < SObjects.getSize(); i++) {
        if ((const plString)"LinkInPointDefault" == SObjects[i]->getName()) {
            //defaultlinkpoint
            printf ("Found one!\nWarping\n\n");

            plSceneObject* linkinpoint = plSceneObject::Convert(SObjects[i]->getObj());
            if (linkinpoint->getCoordInterface().Exists()) {
                plCoordinateInterface* coord = plCoordinateInterface::Convert(linkinpoint->getCoordInterface()->getObj());
                hsMatrix44 mat = coord->getLocalToWorld();
                cam.warp(mat(0,3),mat(1,3),mat(2,3)+5); //raise ourselves up a bit
                float angles = asin(mat(1,0));
                float anglec = acos(mat(0,0));
                if(mat(1,0)<0) anglec = -anglec;
                if(mat(0,0)<0) angles = 3.141592-angles;
                printf("%f %f\n", anglec, angles);
                cam.angle = angles;

                cam.turn();
            }
        }
    }
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
            DrawableList.push_back(dObj);
        }
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

int prpengine::RenderDrawable(DrawableObject* dObj, int rendermode) {
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
        if (dObj->hasCI) {
            objtransform = dObj->CIMat;
        }
        else {
            objtransform = ice->getLocalToWorld();
        }

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
                hsVector3 pos = verts[indice].fPos * objtransform;
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
                                 layer->getAmbient().b, layer->getAmbient().b };
                float dif[4] = { layer->getRuntime().r, layer->getRuntime().g,
                                 layer->getRuntime().b, layer->getRuntime().b };
                float spec[4] = { layer->getSpecular().r, layer->getSpecular().g,
                                  layer->getSpecular().b, layer->getSpecular().b };
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
                    hsVector3 pos = verts[indice].fPos * objtransform;
                    hsVector3 uvw = verts[indice].fUVWs[uvSrc] * layer->getTransform();
                    glTexCoord2f(uvw.X,uvw.Y);
                
                    hsColor32 col = verts[indice].fColor;
                    glColor4ub(col.r,col.g,col.b,col.a);
                    glNormal3f(verts[indice].fNormal.X,verts[indice].fNormal.Y,verts[indice].fNormal.Z);
                    glVertex3f(pos.X,pos.Y ,pos.Z);
                }
                glEnd();
            }
        }
    }
    return 1;
}

void prpengine::UpdateList(hsTArray<plKey> SObjects, bool wireframe) {
    printf("Renderlist Update\n");
    gl_renderlist = glGenLists(SObjects.getSize());
    for (size_t i=0; i < DrawableList.size(); i++) {
        glNewList(gl_renderlist+i, GL_COMPILE);
        RenderDrawable(DrawableList[i],wireframe?0:1);
        glEndList();
    }
}

//bool prpengine::SortDrawables(DrawableObject* lhs, DrawableObject* rhs) {
//    return (lhs->spanflags) < (rhs->spanflags);
//}

//void prpengine::SortDrawableList() {
//	std::sort(DrawableList.begin(), DrawableList.end(), &prpengine::SortDrawables);
//}
void prpengine::draw() {
    for (size_t i=0; i < DrawableList.size(); i++) {
        glCallList(gl_renderlist+i);
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
        if (mipmapimage->getDXCompression() == plBitmap::kDXT1) {
            DXCompressionType = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        }
        else if (mipmapimage->getDXCompression() == plBitmap::kDXT5) {
            DXCompressionType = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        }
        else return 0;
        glBindTexture(GL_TEXTURE_2D, texname); //that thar is the ID
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        for(unsigned int il = 0; il < mipmapimage->getNumLevels(); ++il) {
            glCompressedTexImage2DARB(GL_TEXTURE_2D, il, DXCompressionType, mipmapimage->getLevelWidth(il), mipmapimage->getLevelHeight(il), 0, mipmapimage->getLevelSize(il),(const unsigned long *)mipmapimage->getLevelData(il));
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
