#include "animation.h"

void animation_handler::setAnim(plATCAnim* Anim) {
	Anim_Object = Anim;
}

void animation_handler::PlayAnim(Uint32 currentGlobalTime) {
	animStartPlayTime = currentGlobalTime;
}

void animation_handler::applyCurrentAnimTimeTransform(Uint32 currentGlobalTime) {
    int currentAnimTime = currentGlobalTime - animStartPlayTime;
    float endtime = 3.0; //change this to be the value stated in the Plasma Anim Object
	if (endtime < currentAnimTime)
        currentAnimTime = animStartPlayTime;

//	if (frames[KeyFrameInd+1].time < currentAnimTime) {
//		while (frames[KeyFrameInd+1].time < currentAnimTime) {
//            KeyFrameInd += 1;
//		}
//	}
//    lastframe = frames[KeyFrameInd]
//    nextframe = frames[KeyFrameInd+1]

//    float p1 = lastframe.val;
//    float p2 = nextframe.val;
//    float t1 = lastframe.time;
//    float t2 = nextframe.time;

//    float nowpos = ((p2 - p1)/(t2 - t1)* currentAnimTime) + (p1 - ((p2 - p1)/(t2 - t1) * t1));
}
