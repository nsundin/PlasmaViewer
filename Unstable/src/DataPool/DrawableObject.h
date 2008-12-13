#ifndef __DRAWABLEOBJECT_H__
#define __DRAWABLEOBJECT_H__


#include <stdlib.h>
#include "PRP/KeyedObject/plKey.h"
#include "PRP/Object/plDrawInterface.h"
#include "PRP/Animation/plViewFaceModifier.h"
#include "PRP/Geometry/plClusterGroup.h"
#include "Math/hsMatrix44.h"
#include "EngineObject.h"

class DrawableObject {
public:
	DrawableObject();
	~DrawableObject();
	void * poolinstance;
	EngineObject* ObjOwner;
	plClusterGroup* ClusterGroup;
	bool isCluster;
	plKey SpanKey;
	int DrawableKey;
	int RenderIndex;
	unsigned int spanflags;
	unsigned int renderlevel;
	plDrawInterface* draw;
	plViewFaceModifier* vfm;
};

#endif
