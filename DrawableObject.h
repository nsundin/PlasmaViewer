#ifndef __DRAWABLEOBJECT_H__
#define __DRAWABLEOBJECT_H__


#include <stdlib.h>
#include "Math/hsMatrix44.h"
#include "PRP/KeyedObject/plKey.h"

class DrawableObject {
public:
	bool hasCI;
	hsMatrix44 CIMat;
	plKey Owner;
	plKey SpanKey;
	int DrawableKey;
	unsigned int spanflags;
	unsigned int renderlevel;
};

#endif
