#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>

#include "PRP/Avatar/plAGAnim.h"

class animation_handler {
public:
	int KeyFrameInd;
	void setAnim(plATCAnim* Anim);
	void PlayAnim(Uint32 currentGlobalTime);
	void applyCurrentAnimTimeTransform(Uint32 currentGlobalTime);
	Uint32 animStartPlayTime;
	plATCAnim* Anim_Object;
};

#endif
