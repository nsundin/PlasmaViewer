#ifndef __DATAPOOL_H__
#define __DATAPOOL_H__

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <pthread.h>

#include <SDL/SDL_opengl.h>

#include "PRP/KeyedObject/plKey.h"
#include "PRP/Geometry/plDrawableSpans.h"
#include "PRP/Surface/plLayer.h"
#include "PRP/Surface/plMipmap.h"
#include "PRP/Object/plCoordinateInterface.h"
#include "PRP/Object/plSimulationInterface.h"
#include "PRP/Geometry/plClusterGroup.h"
#include "PRP/Geometry/plCluster.h"
#include "PRP/Geometry/plSpanTemplate.h"

#include "DrawableObject.h"
#include "TextureObject.h"
#include "EngineObject.h"
#include "PhysicalObject.h"
#include "../Msg/EngineMessage.h"
#include "player.h"
#include "camera.h"

bool SortDrawables(DrawableObject* lhs, DrawableObject* rhs);

class DataPool {
public:
	enum MsgCode {
		kErrorMsgNumberLimit,
		kAddedMsg,
	};
	hsMatrix44 getLinkInPointMatrix(); //will return origin can't find linkinpointdefault in current objects
	int loadHeadSpinMipmapTexture(plKey mipmapkey,int texname);
	void DeletePhysicalObject(PhysicalObject* obj);
	void AddPhysicalObject(PhysicalObject* obj);
	void LoadTexturesToGL();
	size_t getNumMsgs();
	void DeleteMessageByInd(int i);
	EngineMessage* getMessage(int i);
	unsigned int RegisterMessage(EngineMessage* msg);
	void DeleteMessage(EngineMessage* msg);
	Player* activePlayer;
	pthread_mutex_t * mutex;
	void SortDrawableList();
	void PrintObjects();
	TextureObject* getTextureObject(size_t ind);
	DrawableObject* getDrawObject(size_t ind);
	size_t getDrawObjectSize();
	size_t getTextureObjectSize();
	Player* getPlayer(int PlayerID);
	Camera* getCamera(int ind);
	Camera* createCamera();
	Camera* getCurrentCamera();
	void SetCurrentCamera(Camera* cam);

    std::vector<TextureObject*> TextureList;
    std::vector<DrawableObject*> DrawableList;
    std::vector<PhysicalObject*> PhysicalObjects;
    std::vector<EngineObject*> EngineObjects;
private:
	Camera* CurrentCamera;
	std::vector<EngineMessage*> EngineMsgs;
	std::vector<Camera*> Cameras;
	std::vector<Player*> Players;
    GLint gl_renderlist;
    GLint gl_rendercount;
    GLuint* gl_texlist;
    hsTArray<plKey> SpawnPoints;
};

#endif
