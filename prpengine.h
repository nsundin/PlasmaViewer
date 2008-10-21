#ifndef __PRPENGINE_H__
#define __PRPENGINE_H__



#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <map>


#include "PRP/Surface/plMipmap.h"
#include "Util/plString.h"
#include "Util/hsTArray.hpp"
#include "PRP/Object/plSceneObject.h"
#include "PRP/KeyedObject/plKey.h"
#include "PRP/Object/plCoordinateInterface.h"
#include "Math/hsMatrix44.h"
#include "DrawableObject.h"
#include "PRP/Object/plDrawInterface.h"
#include "PRP/Geometry/plDrawableSpans.h"
#include "PRP/Surface/plLayer.h"
#include "camera.h"
#include "viewer_fni.h"
#include "DrawableObject.h"
#include "TextureObject.h"


class prpengine {
public:
	void draw();
	void LoadAllTextures(hsTArray<plKey> Textures);
	void UpdateList(hsTArray<plKey> SObjects,bool wireframe);

//	void SortDrawableList();

	void AttemptToSetPlayerToLinkPointDefault(hsTArray<plKey> SObjects,camera &cam);
	void AddSceneObjectToDrawableList(plKey sobjectkey);
	void AppendAllObjectsToDrawList(hsTArray<plKey> SObjects);
	void AttemptToSetFniSettings(plString filename);
private:
	GLint gl_renderlist;
	GLuint* gl_texlist;
	int getTextureIDFromKey(plKey key);
	int loadHeadSpinMipmapTexture(plKey mipmapkey,int texname);

	std::map<plKey, size_t> TexturesGLRef;
	
	std::vector<TextureObject*> TextureList;
	std::vector<DrawableObject*> DrawableList;
//	bool SortDrawables(DrawableObject* lhs, DrawableObject* rhs);
	int RenderDrawable(DrawableObject* dObj, int rendermode);
};

#endif
