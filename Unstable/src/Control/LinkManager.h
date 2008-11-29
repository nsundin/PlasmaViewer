#ifndef __LINKMANAGER_H__
#define __LINKMANAGER_H__

#include "ResManager/plResManager.h"
#include "ResManager/plAgeInfo.h"
#include "PlasmaDefs.h"
#include "Debug/plDebug.h"
#include "../DataPool/DataPool.h"

void PrintProgress(float progresspercent);

class LinkManager {
public:
	DataPool* pool;
	plResManager* rm;

	plString basepath;
	plString datpath;
	plString CurrentAgeName;
	void LoadLocation(const plLocation &loc);
	int Load(const char* filename);
//	void UnloadAge(const plString agename);
//	void Link(plString agename);
};
#endif
