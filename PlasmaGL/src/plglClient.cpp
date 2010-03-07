#include "plglClient.h"

#define WIN_W 800
#define WIN_H 600



plglClient::plglClient(const char *startage) {
    rm = new plResManager(pvPots);
    renderer = new plglRenderer(rm);
    game = new plglGame(rm);
    InitDisplay();
    plDebug::Init(plDebug::kDLAll);
    rm->ReadAge(startage, true);
    //rm->ReadPage(startprp);
    //rm->ReadPage("Personal_District_psnlMYSTII.prp");
    renderer->SetSpanKeys();
    renderer->SetUpAll();

    //set up the cam and stuff
    game->spawnmgr.UpdateSpawnPoints();
    game->spawnmgr.AttemptToSetPlayerToLinkPointDefault(camera);

}

void plglClient::InitDisplay() {
    SDL_VideoInfo *sdlVideoInfo;
    SDL_Surface *screen;
    Uint32 sdlVideoFlags = SDL_OPENGL;
    int sdlbpp = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    atexit(SDL_Quit);

    sdlVideoInfo = (SDL_VideoInfo *)SDL_GetVideoInfo();

    if (sdlVideoInfo->hw_available)
        sdlVideoFlags |= SDL_HWSURFACE;
    else
        sdlVideoFlags |= SDL_SWSURFACE;

    if (sdlVideoInfo->blit_hw)
        sdlVideoFlags |= SDL_HWACCEL;

    sdlbpp = sdlVideoInfo->vfmt->BitsPerPixel;

    if (!(screen = SDL_SetVideoMode(WIN_W, WIN_H, sdlbpp, sdlVideoFlags))) {
        fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void plglClient::ProcessEvents(Uint32 timefactor) {
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
            break;
        }
        else if (event.type == SDL_KEYDOWN) { 
            switch(event.key.keysym.sym){
                case SDLK_x:
                    camera.moveLocalY(1.0);
                    break;
            case SDLK_z:
                    camera.moveLocalY(-1.0);
                    break;
                case SDLK_l:
                    if (renderer->icanhaslightz)
                        renderer->icanhaslightz = false;
                    else if (!renderer->icanhaslightz)
                        renderer->icanhaslightz = true;
                default:
                  break;
            }
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN) { 
            if (event.button.button == SDL_BUTTON_LEFT) {
                SDL_GetMouseState(&oldmouseX,&oldmouseY);
            }
        }
        else if (event.type == SDL_MOUSEMOTION) {
            if (SDL_GetMouseState(&mouseX,&mouseY)&SDL_BUTTON(1)) {
                camera.moveLocalZ((mouseY-oldmouseY)*(float(timefactor)/5000));
                camera.turn((mouseX-oldmouseX)*(float(timefactor)/5000));
            }
        }
    }
}
void plglClient::doMain() {
    Uint32 last_time = 0;
    Uint32 now_time;
    Uint32 timefactor;
    renderer->SetUpRenderer();

    while (1) { //main engine loop
//		printf("%f,%f,%f\n",camera.GetPosition()[0],camera.GetPosition()[1],camera.GetPosition()[2]);
        now_time = SDL_GetTicks();
        timefactor = now_time-last_time;
        last_time = now_time;
        renderer->Render(&camera);
        ProcessEvents(timefactor);
        camera.update();
    }
}
