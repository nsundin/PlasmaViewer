#ifndef __PRPENGINE_H__
#define __PRPENGINE_H__

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>

#include <SDL/SDL_opengl.h>
#include <GL/glext.h>

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
#include "PRP/Geometry/plClusterGroup.h"
#include "PRP/Geometry/plCluster.h"
#include "PRP/Geometry/plSpanTemplate.h"
#include "camera.h"
#include "viewer_fni.h"
#include "DrawableObject.h"
#include "TextureObject.h"
#include "maths.h"

class prpengine {
public:
    void UnloadObjects(int sequenceprefix);
    void draw(camera &cam);
    void LoadTextures(std::vector<plKey> Textures);
    void UpdateList(bool wireframe);

    void SortDrawableList();
    void PrintObjects();

    void AddClusterGroupToDrawableList(plKey clustergroupkey);
    void AppendClustersToDrawList(std::vector<plKey> clusters);

    void AddSceneObjectToDrawableList(plKey sobjectkey);
    void AppendObjectsToDrawList(std::vector<plKey> SObjects);


    void AttemptToSetFniSettings(plString fnipath);

    void AttemptToSetPlayerToLinkPointDefault(camera &cam);
    void UpdateSpawnPoints();
    void NextSpawnPoint(camera &cam);
    void PrevSpawnPoint(camera &cam);
    bool SetSpawnPoint(plString name, camera &cam);
    std::vector<plKey> AllLoadedSceneObjects;
private:
    std::vector<TextureObject*> TextureList;
    std::vector<DrawableObject*> DrawableList;
    
    GLint gl_renderlist;
    GLint gl_rendercount;

    GLuint* gl_texlist;
    int getTextureIDFromKey(plKey key);
    int loadHeadSpinMipmapTexture(plKey mipmapkey,int texname);

    hsTArray<plKey> SpawnPoints;
    int curSpawnPoint;
    void renderClusterMesh(hsTArray<plSpanTemplate::Vertex> verts, const unsigned short* indices, int NumTris,hsGMaterial* material);
    void renderSpanMesh(hsTArray<plGBufferVertex> verts, hsTArray<unsigned short> indices,hsGMaterial* material,bool isWaveset, float WaterHeight);
    void SetLayerParams(plLayerInterface* layer, bool isWaveset);
    int RenderDrawable(DrawableObject* dObj, int rendermode, camera &cam);
    void VFM_Spherical(float *cam, float *worldPos);
    template <class T>
    T *getModifierOfType(plSceneObject *sObj, T*(type)(plCreatable *pCre));
    void SetSpawnPoint(int idxc, camera &cam);
};
extern  camera cam;
#endif