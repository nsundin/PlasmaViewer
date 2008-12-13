#ifndef __LINKMANAGER_H__
#define __LINKMANAGER_H__

#include "ResManager/plResManager.h"
#include "ResManager/plAgeInfo.h"
#include "PlasmaDefs.h"
#include "Debug/plDebug.h"
#include "../SDLWindow.h"
#include "../DataPool/DataPool.h"
#include "../Physics/PhysicsEngine.h"

class LinkManager {
public:
	SDLWindow* window;
	DataPool* pool;
	plResManager* rm;

	plString basepath;
	plString datpath;
	plString CurrentAgeName;
//	static void UpdateProgressBar(float progresspercent);
	static void PrintProgress(float progresspercent);
	void LoadLocation(const plLocation &loc);
	int Load(const char* filename);

	void LoadTextures(std::vector<plKey> Textures);
	void AddObject(plKey sobjectkey);
	void AppendClusterGroupToDrawList(plKey clustergroupkey);
	void AppendClustersToDrawList(std::vector<plKey> clusters);
	void AddObjects(std::vector<plKey> SObjects);

//	void UnloadAge(const plString agename);
//	void Link(plString agename);
};
#endif
