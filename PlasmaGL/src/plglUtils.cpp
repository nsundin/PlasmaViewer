#include "Math/hsMatrix44.h"


static float* glMatrix44_from_hsMatrix44(hsMatrix44 data) {
    static float mat[16];
    mat[ 0] = data(0,0);
    mat[ 1] = data(1,0);
    mat[ 2] = data(2,0);
    mat[ 3] = data(3,0);
    mat[ 4] = data(0,1);
    mat[ 5] = data(1,1);
    mat[ 6] = data(2,1);
    mat[ 7] = data(3,1);
    mat[ 8] = data(0,2);
    mat[ 9] = data(1,2);
    mat[10] = data(2,2);
    mat[11] = data(3,2);
    mat[12] = data(0,3);
    mat[13] = data(1,3);
    mat[14] = data(2,3);
    mat[15] = data(3,3);
    return mat;
}

