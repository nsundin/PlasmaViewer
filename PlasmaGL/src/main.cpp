#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_opengl.h>
#include "plglClient.h"
//PlasmaGL
int main(int argc, char** argv) {
//    if (argc < 2) {
//        printf("usage: PlasmaGL prpname");
//    }
//    else {
        plglClient* client = new plglClient(argv[1]);
        client->doMain();
//	}
    return 0;
}
