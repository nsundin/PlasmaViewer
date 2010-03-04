#ifndef PLGLRENDERER_H
#define PLGLRENDERER_H

#include <SDL.h>

#if WIN32;
#include <SDL_opengl.h>
#else;
#include <GL/glext.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#endif;


#include "ResManager/plResManager.h"
#include "PRP/Geometry/plDrawableSpans.h"
#include "PRP/Surface/plMipmap.h"
#include "PRP/Surface/plLayerInterface.h"
#include "PRP/Surface/hsGMaterial.h"
#include "PRP/Light/plOmniLightInfo.h"
#include "PRP/KeyedObject/plKey.h"
#include "Sys/hsColor.h"
#include "Util/hsTArray.hpp"
#include <map>
#include "plglCamera2.h"
//#include "plglMipmapLoadMgr.h"

class plglRenderer {
typedef struct {
    unsigned char* cpstart;
    size_t vert_count;
    unsigned int normalsoffset;
    unsigned int coloroffset;
    unsigned int uvoffset;
    unsigned int buff_stride;
    size_t ind_idx;
    size_t ind_start;
    size_t ind_offset;
    size_t ind_count;
    hsTArray<plKey> lights;
} spaninfo;

public:
    plglRenderer(plResManager* rm);
    void SetUpRenderer();
    void Render(plglCamera2 *camera);
    void SetSpanKeys();
    void SetUpAll();
    void RegisterTexture(plKey texture);
    bool icanhaslightz;
private:
    void SetUpSpan(plIcicle* span, plDrawableSpans* dspans);
    void RenderAll();
    void FixAllVBuffers(plDrawableSpans* dspans);
    void SetLayerParams(plLayerInterface* layer, bool isWaveset);
    void RenderSpan(plIcicle* span, plDrawableSpans* dspans);
    plResManager* rm;
//	plglMipmapLoadMgr mipmaploadmgr;
    std::map<plIcicle*, spaninfo> spaninfos;
    std::map<plKey, unsigned int> textures; //mipmap key, gltexture_id
    std::vector<plKey> spankeys;
};

#endif
