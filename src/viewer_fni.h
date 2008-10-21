#ifndef __VIEWER_FNI_H__
#define __VIEWER_FNI_H__

#include "Util/plString.h"
#include "Stream/plEncryptedStream.h"
#include "Util/hsTArray.hpp"

#include <iostream>
#include <stdio.h>

class fni {
public:
	enum {
        kNone = 0,
		kLinear = 1,
		kExp2 = 3
    };

	int fFogType;
	float fYon;
	float fDefLinear[3];
	float fDefExp2[2];
	float fClearColor[3];
	float fDefColor[3];
	int load(plString filename);
};
#endif
