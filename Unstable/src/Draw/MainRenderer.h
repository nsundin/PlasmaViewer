#ifndef __MAINRENDERER_H__
#define __MAINRENDERER_H__

#include <SDL/SDL_opengl.h>
#include "PRP/Object/plDrawInterface.h"
#include "PRP/Surface/plLayerInterface.h"
#include "PRP/Surface/hsGMaterial.h"
#include "PRP/Geometry/plSpanTemplate.h"
#include "PRP/Geometry/plDrawableSpans.h"
#include "../DataPool/DrawableObject.h"
#include "../DataPool/TextureObject.h"
#include "../DataPool/camera.h"
#include "../DataPool/DataPool.h"
#include "../maths.h"
#include "Math/hsMatrix44.h"

class MainRenderer {
public:
	DataPool* pool;
	void LoadTexturesToGL();
	void SetCamera(Camera* cam);

	void VFM_Spherical(float *cam, float *worldPos);
	int RenderDrawable(DrawableObject* dObj, int rendermode);
	void renderClusterMesh(hsTArray<plSpanTemplate::Vertex> verts, const unsigned short* indices, int NumTris, hsGMaterial* material);
	void renderSpanMesh(hsTArray<plGBufferVertex> verts, hsTArray<unsigned short> indices,hsGMaterial* material,bool isWaveset, float WaterHeight);
	unsigned int SetLayerParams(plLayerInterface* layer, bool isWaveset);
	void UpdateList(bool wireframe);
	void draw();
	int getTextureIDFromKey(plKey in_key);
    GLint gl_renderlist;
    GLint gl_rendercount;
    GLuint* gl_texlist;

};

#endif
