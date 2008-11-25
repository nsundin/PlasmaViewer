#include "MainRenderer.h"

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

int MainRenderer::RenderDrawable(DrawableObject* dObj, int rendermode) {
    if (!dObj->isCluster) {
        plDrawableSpans* span = plDrawableSpans::Convert(dObj->SpanKey->getObj());
        plDISpanIndex di = span->getDIIndex(dObj->DrawableKey);
        if ((di.fFlags & plDISpanIndex::kMatrixOnly) != 0) {
            return 0;
        }
        if(dObj->draw->getProperties().get(0)) return 0;
        for (size_t idx=0; idx<di.fIndices.getSize(); idx++) {
            plIcicle* ice = (plIcicle*)span->getSpan(di.fIndices[idx]);
            //searching for a honest matrix... oh no it's not anymore.  what a pity =P
            plKey materialkey = span->getMaterial(ice->getMaterialIdx());
            if (!materialkey.isLoaded())
                return 0;
            hsGMaterial* material = hsGMaterial::Convert(materialkey->getObj());
            renderSpanMesh(span->getVerts(ice),span->getIndices(ice),material,(ice->getProps() & plSpan::kWaterHeight),ice->getWaterHeight());
        }
    }
    else {
        plKey materialkey = dObj->ClusterGroup->getMaterial();
        if (!materialkey.isLoaded())
            return 0;
        hsGMaterial* material = hsGMaterial::Convert(materialkey->getObj());

        plSpanTemplate span = dObj->ClusterGroup->getTemplate();
        renderClusterMesh(span.getVertices(),span.getIndices(),span.getNumTris(),material);
    }
    return 1;
}



void MainRenderer::renderClusterMesh(hsTArray<plSpanTemplate::Vertex> verts, const unsigned short* indices, int NumTris, hsGMaterial* material) {
    for (size_t layeridx = 0; layeridx < material->getNumLayers(); layeridx++) {
        plKey layerkey = material->getLayer(layeridx);
        plLayerInterface* layer = plLayerInterface::Convert(layerkey->getObj());
        size_t uvSrc = layer->getUVWSrc() & 0xFFFF;
        SetLayerParams(layer,false);
        //now our mesh
        glBegin(GL_TRIANGLES);
        for (size_t j=0; j < (size_t)(NumTris * 3); j++) {
            int indice = indices[j];
            hsVector3 pos;
            pos = verts[indice].fPosition;		
            hsVector3 uvw = verts[indice].fUVWs[uvSrc] * layer->getTransform();
            glTexCoord2f(uvw.X,uvw.Y);
            hsColor32 col = verts[indice].fColor1;
            glColor4ub(col.r,col.g,col.b,col.a==1?255:col.a);
            glNormal3f(verts[indice].fNormal.X,verts[indice].fNormal.Y,verts[indice].fNormal.Z);
            glVertex3f(pos.X,pos.Y ,pos.Z);
        }
        glEnd();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}

void MainRenderer::renderSpanMesh(hsTArray<plGBufferVertex> verts, hsTArray<unsigned short> indices,hsGMaterial* material,bool isWaveset, float WaterHeight) {
    for (size_t layeridx = 0; layeridx < material->getNumLayers(); layeridx++) {
        plKey layerkey = material->getLayer(layeridx);
        plLayerInterface* layer = plLayerInterface::Convert(layerkey->getObj());
        size_t uvSrc = layer->getUVWSrc() & 0xFFFF;
        SetLayerParams(layer,isWaveset);
        glBegin(GL_TRIANGLES);
        for (size_t j = 0; j < indices.getSize(); j++) {
            int indice = indices[j];
            hsVector3 pos;
            pos = verts[indice].fPos;		
            hsVector3 uvw = verts[indice].fUVWs[uvSrc] * layer->getTransform();
            glTexCoord2f(uvw.X,uvw.Y);

            hsColor32 col = verts[indice].fColor;
            glColor4ub(col.r,col.g,col.b,col.a==1?255:col.a);
            
            glNormal3f(verts[indice].fNormal.X,verts[indice].fNormal.Y,verts[indice].fNormal.Z);
            if (isWaveset) {
                glColor3ub(col.r,col.g,col.b);
                glVertex3f(pos.X,pos.Y, WaterHeight);
            }
            else
                glVertex3f(pos.X,pos.Y ,pos.Z);
        }
        glEnd();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}

unsigned int MainRenderer::SetLayerParams(plLayerInterface* layer, bool isWaveset) {
    if (layer->getTexture()) {
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
    if (is2Sided || isWaveset) {
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
//	glPixelTransferf(GL_ALPHA_SCALE, layer->getOpacity());

    glDisable(GL_BLEND);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    if (layer->getState().fBlendFlags & hsGMatState::kBlendAlpha) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
    if (layer->getState().fBlendFlags & hsGMatState::kBlendAdd) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
	if (layer->getState().fBlendFlags & hsGMatState::kBlendMult) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE_MINUS_SRC_COLOR,GL_SRC_COLOR);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
//	if (layer->getState().fBlendFlags & 0x00024002) {
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_DST_COLOR,GL_ZERO);
//		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//	}
    if (layer->getState().fBlendFlags & hsGMatState::kBlendNoTexColor) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    }
	return layer->getState().fMiscFlags;
}

void MainRenderer::VFM_Spherical(float *cam, float *worldPos) {
    float lookAt[3]={0,-1,0},objToCamProj[3],objToCam[3],upAux[3],angleCosine;
    objToCamProj[0] = cam[0] - worldPos[0] ;
    objToCamProj[1] = cam[1] - worldPos[1] ;
    objToCamProj[2] = 0 ;

    mathsNormalize(objToCamProj);
    mathsCrossProduct(upAux,lookAt,objToCamProj);

    angleCosine = mathsInnerProduct(lookAt,objToCamProj);

    if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
        glRotatef(acos(angleCosine)*180/3.14,upAux[0], upAux[2], upAux[1]);

    objToCam[0] = cam[0] - worldPos[0] ;
    objToCam[1] = cam[1] - worldPos[1] ;
    objToCam[2] = cam[2] - worldPos[2] ;

    mathsNormalize(objToCam);
    angleCosine = mathsInnerProduct(objToCamProj,objToCam);

    if ((angleCosine < 0.99990) && (angleCosine > -0.9999)) {
        if (objToCam[2] < 0)
            glRotatef(acos(angleCosine)*180/3.14,-1,0,0);
        else
            glRotatef(acos(angleCosine)*180/3.14,1,0,0);
    }

}

void MainRenderer::UpdateList(bool wireframe) {
    printf("Renderlist Update\n");
    int count = 0;
    if(gl_rendercount)
        glDeleteLists(gl_renderlist, gl_rendercount);

    gl_rendercount = pool->getDrawObjectSize();
    gl_renderlist = glGenLists(gl_rendercount);
    for (size_t i=0; i < pool->getDrawObjectSize(); i++) {
        pool->getDrawObject(i)->RenderIndex =  i;
        glNewList(gl_renderlist+i, GL_COMPILE);
        RenderDrawable(pool->getDrawObject(i),wireframe?0:1);
        glEndList();
        count++;
    }
    printf("Regenerated %d items\n", count);
}

void MainRenderer::draw() {
    glPushMatrix();
    pool->SortDrawableList();
	
    for (size_t i=0; i < pool->getDrawObjectSize(); i++) {
        DrawableObject *dObj = pool->getDrawObject(i);
        if (dObj->isCluster) {
            for (size_t clusteridx = 0; clusteridx < dObj->ClusterGroup->getNumClusters(); clusteridx++) {
                plCluster* c = dObj->ClusterGroup->getCluster(clusteridx);
                for(size_t inst = 0; inst < c->getNumInstances(); inst++) {
                    glPushMatrix();
                    glMultMatrixf(getMatrixFrom_hsMatrix44(c->getInstance(inst)->getLocalToWorld()));
                    glCallList(gl_renderlist+dObj->RenderIndex);
                    glPopMatrix();
                }
            }
        }
        else {
            glPushMatrix();
            if (dObj->hasCI) {
                glMultMatrixf(getMatrixFrom_hsMatrix44(pool->getDrawObject(i)->CIMat));
            }
            if(dObj->vfm) {
                if(dObj->vfm->getFlag(plViewFaceModifier::kFaceCam)) {
                    if(dObj->hasCI) {
                        float camV[3], objV[3];
                        for(int i = 0; i < 3; i++) {
                            camV[i] = pool->getCurrentCamera()->getCamPos(i);
                            objV[i] = dObj->CIMat(i,3);
                        }
                        VFM_Spherical(camV, objV);
                    }
                }
            }
            glCallList(gl_renderlist+dObj->RenderIndex);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

int MainRenderer::getTextureIDFromKey(plKey in_key) {
    for (size_t imageidx = 0; imageidx < pool->getTextureObjectSize(); imageidx++) {
        if (pool->getTextureObject(imageidx)->key->getName() == in_key->getName()) {

            return pool->getTextureObject(imageidx)->textureInd;
        }
    }
    return -1;
}
