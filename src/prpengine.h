#ifndef __PRPENGINE_H__
#define __PRPENGINE_H__


#ifdef WIN32
#include <windows.h>
#else
#include <GL/glx.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <algorithm>
#include <vector>

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
#include "PRP/Modifier/plSpawnModifier.h"
#include "camera.h"
#include "viewer_fni.h"
#include "DrawableObject.h"
#include "TextureObject.h"
#include "maths.h"

class prpengine {
public:
	void draw(camera &cam);
	void LoadAllTextures(hsTArray<plKey> Textures);
	void UpdateList(hsTArray<plKey> SObjects,bool wireframe, bool firstTime, camera &cam);

	void SortDrawableList();
	void PrintObjects();

	void AttemptToSetPlayerToLinkPointDefault(hsTArray<plKey> SObjects,camera &cam);
	void AddSceneObjectToDrawableList(plKey sobjectkey);
	void AppendAllObjectsToDrawList(hsTArray<plKey> SObjects);
	void AttemptToSetFniSettings(plString filename);
	void NextSpawnPoint(camera &cam);
	void PrevSpawnPoint(camera &cam);
	bool SetSpawnPoint(plString name, camera &cam);
	//bool SortDrawables(DrawableObject* lhs, DrawableObject* rhs);
private:
	GLint gl_renderlist;
	GLint gl_rendercount;
	GLuint* gl_texlist;
	int getTextureIDFromKey(plKey key);
	int loadHeadSpinMipmapTexture(plKey mipmapkey,int texname);
	
	std::vector<TextureObject*> TextureList;
	std::vector<DrawableObject*> DrawableList;
	hsTArray<plKey> SpawnPoints;
	int curSpawnPoint;
	int RenderDrawable(DrawableObject* dObj, int rendermode, camera &cam);
	void l3dBillboardSphericalBegin(float *cam, float *worldPos);
	template <class T>
	T *getModifierOfType(plSceneObject *sObj, T*(type)(plCreatable *pCre));
	void SetSpawnPoint(int idxc, camera &cam);
};
extern  camera cam;
#endif
