#ifndef __DRAWABLEOBJECT_H__
#define __DRAWABLEOBJECT_H__


#include <stdlib.h>
#include "Math/hsMatrix44.h"
#include "PRP/KeyedObject/plKey.h"
#include "PRP/Object/plDrawInterface.h"
#include "PRP/Animation/plViewFaceModifier.h"

class DrawableObject {
public:
	plKey ClusterKey;
	bool isCluster;

	bool isAnimPlaying;
	bool hasCI;
	hsMatrix44 CIMat;
	plKey Owner;
	plKey SpanKey;
	int DrawableKey;
	int RenderIndex;
	unsigned int spanflags;
	unsigned int renderlevel;
	plDrawInterface* draw;
	plViewFaceModifier* vfm;
};

#endif
