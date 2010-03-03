#include "plglRenderer.h"
#include "plglUtils.cpp"

#ifdef WIN32
  #include <GL/wglext.h>
//#else
//  #include <GL/glxext.h>
#endif

#define BGRA2RGBA(val) \
    ((val & 0x000000FF) << 16 | (val & 0x00FF0000) >> 16 | (val & 0xFF00FF00))

PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = NULL;

plglRenderer::plglRenderer(plResManager* rm){
    this->rm = rm;
    //this should really be set per-object but should be fine until materials get set up
#ifdef WIN32
    glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)wglGetProcAddress("glCompressedTexImage2DARB");
#else
    glCompressedTexImage2DARB = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)glXGetProcAddress((const GLubyte*)"glCompressedTexImage2DARB");
#endif
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void plglRenderer::SetUpRenderer() {
    glViewport(0,0,(GLsizei)800,(GLsizei)600);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (double)800 / (double)600, 1.0f, 1000000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}
void plglRenderer::Render(plglCamera2 *camera) {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    RenderAll();
    glPopMatrix();
    SDL_GL_SwapBuffers();
}



void plglRenderer::RenderAll() {
    for (size_t si=0; si < spankeys.size(); ++si) {
        plDrawableSpans* dspans = plDrawableSpans::Convert(spankeys[si]->getObj());
        for (size_t span_i=0; span_i < dspans->getNumSpans(); span_i++) {
            RenderSpan((plIcicle*)dspans->getSpan(span_i),dspans);
        }
    }
}

void plglRenderer::SetUpAll() {
    std::vector<plLocation> locs = rm->getLocations();
    for (size_t i=0; i < locs.size(); ++i) {
        std::vector<plKey> mms = rm->getKeys(locs[i], kMipmap);
        for (size_t i2=0; i2 < mms.size(); ++i2) {
            RegisterTexture(mms[i2]);
        }
    }
    

    for (size_t si=0; si < spankeys.size(); ++si) {
        plDrawableSpans* dspans = plDrawableSpans::Convert(spankeys[si]->getObj());
        for (size_t span_i=0; span_i < dspans->getNumSpans(); span_i++) {
            FixAllVBuffers(dspans);
            SetUpSpan((plIcicle*)dspans->getSpan(span_i),dspans);
        }
    }
}

void plglRenderer::SetUpSpan(plIcicle* span, plDrawableSpans* dspans) {
    plglRenderer::spaninfo newspaninfo;
    plGBufferGroup* buff = dspans->getBuffer(span->getGroupIdx());
//vertex stuff
    size_t vert_idx = span->getVBufferIdx();
    size_t vert_start = span->getVStartIdx();
    size_t vert_count = span->getVLength();
    unsigned char* cp = (unsigned char*)buff->getMutableVertBuffer(vert_idx) + (buff->getStride() * vert_start);
    if (vert_count == (size_t)-1)
        vert_count = (buff->getVertBufferSize(vert_idx) / buff->getStride()) - vert_start;

//index stuff
    size_t ind_idx = span->getIBufferIdx();
    size_t ind_start = span->getIStartIdx();
    size_t ind_offset = span->getVStartIdx();
    size_t ind_count = span->getILength();
    if (ind_count == (size_t)-1)
        ind_count = buff->getIdxBufferCount(span->getIBufferIdx()) - ind_start;
    
    newspaninfo.ind_idx = ind_idx;
    newspaninfo.ind_start = ind_start;
    newspaninfo.ind_offset = ind_offset;
    newspaninfo.ind_count = ind_count;
    newspaninfo.vert_count = vert_count;


    unsigned int numUVs = buff->getFormat() & plGBufferGroup::kUVCountMask;

    unsigned int normalsoffset = sizeof(float)*3;
    int weightCount = (buff->getFormat() & plGBufferGroup::kSkinWeightMask) >> 4;
    if (weightCount > 0) {
        normalsoffset += sizeof(float)*weightCount;
        if (buff->getFormat() & plGBufferGroup::kSkinIndices) {
            normalsoffset += sizeof(int);
        }
    }
    unsigned int coloroffset = normalsoffset+(sizeof(float)*3);
    newspaninfo.buff_stride = buff->getStride();
    newspaninfo.cpstart = cp;
    newspaninfo.coloroffset = coloroffset;
    newspaninfo.normalsoffset = normalsoffset;
    newspaninfo.uvoffset = buff->getStride()-((buff->getFormat() & plGBufferGroup::kUVCountMask)*sizeof(float)*3); //GIGANTIC HACK
        

    spaninfos[span] = newspaninfo;
}

void plglRenderer::FixAllVBuffers(plDrawableSpans* dspans) {
    for (size_t i=0; i < dspans->getNumBufferGroups(); i++) {
        plGBufferGroup* buff = dspans->getBuffer(i);
        for (size_t jb=0; jb < buff->getNumVertBuffers(); jb++) {
            unsigned int count = buff->getVertBufferSize(jb)/buff->getStride();
            unsigned char* cp = buff->getMutableVertBuffer(jb);
            for (size_t iv=0; iv<count; iv++) {
                cp += (sizeof(float)*3);//pos
                int weightCount = (buff->getFormat() & plGBufferGroup::kSkinWeightMask) >> 4;
                if (weightCount > 0) {
                    for (int j=0; j<weightCount; j++) {
                        cp += sizeof(float);
                    }
                    if (buff->getFormat() & plGBufferGroup::kSkinIndices) {
                        cp += sizeof(int);
                    }
                }
                cp += (sizeof(float)*3); //normal
                //fix the colors  GBRA > RGBA
                *(unsigned int*)cp = BGRA2RGBA(*(unsigned int*)cp);//color
                cp += sizeof(unsigned int);
                cp += sizeof(unsigned int);
                for (size_t j=0; j<(size_t)(buff->getFormat() & plGBufferGroup::kUVCountMask); j++) {
                    cp += (sizeof(float)*3);//UV
                }
            }
        }
    }
}

void plglRenderer::SetLayerParams(plLayerInterface* layer, bool isWaveset) {
    if (layer->getTexture()) {
        if (layer->getTexture().isLoaded()) {
            unsigned int texID = textures[layer->getTexture()];
            if (texID) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, texID);
            }
            else {
                //printf("Bad texture: %s\n", layer->getTexture()->getName().cstr());
                glDisable(GL_TEXTURE_2D);
            }
        }
        else {
           // printf("Texture not loaded: %s\n", layer->getTexture()->getName().cstr());
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
    if ((layer->getState().fBlendFlags & hsGMatState::kBlendNoTexColor) != 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    }
}


void plglRenderer::RenderSpan(plIcicle* span, plDrawableSpans* dspans) {
    spaninfo si = spaninfos[span];
    plGBufferGroup* buff = dspans->getBuffer(span->getGroupIdx());

    glPushMatrix();
    glMultMatrixf(glMatrix44_from_hsMatrix44(span->getWorldToLocal()));

    hsGMaterial* material = hsGMaterial::Convert(dspans->getMaterial(span->getMaterialIdx())->getObj());
    for (size_t i=0; i < material->getNumLayers(); i++) {
        plLayerInterface* layer = plLayerInterface::Convert(material->getLayer(i)->getObj());
        SetLayerParams(layer, false);

        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_VERTEX_ARRAY);

        glNormalPointer(GL_FLOAT, si.buff_stride, (GLvoid*)(si.cpstart+si.normalsoffset));
        glColorPointer(4, GL_UNSIGNED_BYTE, si.buff_stride, (GLvoid*)(si.cpstart+si.coloroffset));
        glTexCoordPointer(3, GL_FLOAT,si.buff_stride, (GLvoid*)(si.cpstart+si.uvoffset)); //+(sizeof(float)*layer->getUVWSrc())
        glVertexPointer(3, GL_FLOAT, si.buff_stride, (GLvoid*)si.cpstart);


        glBegin(GL_TRIANGLES);
        for (size_t j=0; j < si.ind_count; j++) {
            unsigned short ind = buff->getIdxBufferStorage(si.ind_idx)[j + si.ind_start] - si.ind_offset;
            glArrayElement(ind);
        }
        glEnd();

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
    }
    glPopMatrix();
}

void plglRenderer::SetSpanKeys() {
    std::vector<plLocation> locs = rm->getLocations();
//	printf("Adding %i location(s)\n", locs.size());
    for (size_t i=0; i < locs.size(); ++i) {
        std::vector<plKey> sk = rm->getKeys(locs[i], kDrawableSpans);
        for (size_t i2=0; i2 < sk.size(); ++i2) {
            spankeys.push_back(sk[i2]);
        }
    }
}
int loadMipmapTexture(plKey mipmapkey, unsigned int texname) {
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        for(unsigned int il = 0; il < mipmapimage->getNumLevels(); ++il) {
            glCompressedTexImage2DARB(GL_TEXTURE_2D, il, DXCompressionType, mipmapimage->getLevelWidth(il), mipmapimage->getLevelHeight(il), 0, mipmapimage->getLevelSize(il),(const unsigned long *)mipmapimage->getLevelData(il));
        }
        return 1;
    } else {
        for (unsigned int il = 0; il < mipmapimage->getNumLevels(); ++il) {
            glBindTexture(GL_TEXTURE_2D, texname);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, il, GL_RGBA, mipmapimage->getLevelWidth(il), mipmapimage->getLevelHeight(il), 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, mipmapimage->getLevelData(il));
        }
        return 1;
    }
        return 0;
}


void plglRenderer::RegisterTexture(plKey texture) {
    if (textures[texture]) { //if it's already there
        printf("%s already exists: skipping load.\n",texture->getName().cstr());
        return;
    }
    unsigned int texid;
    glGenTextures(1, &texid);
    if (!loadMipmapTexture(texture, texid)) {
        glDeleteTextures(1,&texid);
        printf("Warning: %s failed to load.\n",texture->getName().cstr());
    }
    else {
        //celebrate
        textures[texture] = texid;
        printf("Loaded %s\n",texture->getName().cstr());
    }
}
