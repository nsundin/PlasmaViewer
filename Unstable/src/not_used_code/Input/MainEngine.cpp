#include "MainEngine.h"

//#define datfolder "dat"
//#define sdlfolder "SDL"
//#define pythonfolder "Python"
//#define soundsfolder "sfx"

void MainEngine::Init() {
	lnkmgr.prp_engine = &prp_engine;
	GL_init();
}

void MainEngine::GL_init() {
    glDisable(GL_LIGHTING);
    glClearDepth(1.0f);
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearAccum(0.0f, 0.0f, 0.0f, 0.0f);
}

void MainEngine::KeyDownTrue(bool* var, unsigned press_type) {
    if (press_type == SDL_KEYDOWN)
        *var = true;
    else
        *var = false;
}
void MainEngine::KeyCallback(SDL_keysym* keysym,unsigned int type) {
    switch(keysym->sym) {
        case SDLK_ESCAPE:
            if (type == SDL_KEYDOWN)
                plasmaconsole.escape();
            break;
 //Player Control Keys
        case SDLK_LEFT:
            if (plasmaconsole.isHighlighted && type == SDL_KEYDOWN)
                plasmaconsole.CursorMoveLeft();
            else
                KeyDownTrue(&currentPlayer.isTurningLeft,type);
            break;
        case SDLK_RIGHT:
            if (plasmaconsole.isHighlighted && type == SDL_KEYDOWN) 
                plasmaconsole.CursorMoveRight();
            else
                KeyDownTrue(&currentPlayer.isTurningRight,type);
            break;
        case SDLK_UP:
            KeyDownTrue(&currentPlayer.isMovingForward,type);
            break;
        case SDLK_DOWN:
            KeyDownTrue(&currentPlayer.isMovingBackward,type);
            break;
        case SDLK_LSHIFT:
            KeyDownTrue(&currentPlayer.isRun,type);
            break;
        case SDLK_RSHIFT:
            KeyDownTrue(&currentPlayer.isRun,type);
            break;
        case SDLK_x:
            if (!plasmaconsole.isHighlighted)
                KeyDownTrue(&currentPlayer.isMovingDown,type);
            break;
        case SDLK_z:
            if (!plasmaconsole.isHighlighted)
                KeyDownTrue(&currentPlayer.isMovingUp,type);
            break;

//when all of these features in the console this block can be deleted safely
        //case SDLK_d:
        //    if (type == SDL_KEYDOWN)
        //        printf("You are facing %f rads\n",cam.angle);
        //    break;
        //case SDLK_l:
        //    if (type == SDL_KEYDOWN)
        //        printf("You are at (%f, %f, %f)\n",cam.getCamPos(0), cam.getCamPos(1),cam.getCamPos(2));
        //    break;
        //case SDLK_p:
        //    if (type == SDL_KEYDOWN)
        //        prp_engine.PrintObjects();
        //    break;
        //case SDLK_s:
        //    if (type == SDL_KEYDOWN) {
        //        if (currentPlayer.isRun) prp_engine.PrevSpawnPoint(cam);
        //        else prp_engine.NextSpawnPoint(cam);
        //    }
        //    break;
//        case SDLK_w:
//            if (type == SDL_KEYDOWN){
//                wireframe = !wireframe;
//                prp_engine.UpdateList(wireframe,true,cam);
//            }
//            break;
        default:
            break;
    }
}

void MainEngine::ProcessConsoleCommand(const char * text) {
    printf("%s\n",text);
    //link just prints for now
    if (plString(text).startsWith("/link")) {
        if (plString(text).split(' ').size() < 2) {
            printf("You're linking to D'ni...  HA, not so quick. :F\n");		
            return;
        }
        plString location = plString(text).split(' ')[1];
        printf("Location: %s\n",location.cstr());
    }
    else if (plString(text).startsWith("/unload")) {
        if (plString(text).split(' ').size() < 2) 
            return;
        plString agename = plString(text).split(' ')[1];
        lnkmgr.UnloadAge(agename);
    }
    else if (plString(text).startsWith("/load")) {
        if (plString(text).split(' ').size() < 2) 
            return;
        plString filename = plString(text).split(' ')[1];
        lnkmgr.Load(filename);
        window->resize();
    }
    else if (plString(text) == "/spawn") {
        prp_engine.NextSpawnPoint(cam);
    }
    else if (plString(text) == "/spawnlast") {
        prp_engine.PrevSpawnPoint(cam);
    }
    else if (plString(text) == "/updatelist") {
        prp_engine.UpdateList(false);
    }
    else if (plString(text).startsWith("/setfni")) {
        if (plString(text).split(' ').size() < 2) 
            return;
        plString filename = plString(text).split(' ')[1];
        prp_engine.AttemptToSetFniSettings(filename);
    }
}

void MainEngine::ProcessEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            KeyCallback(&event.key.keysym,event.type);
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE) plasmaconsole.backSpace();
                if (event.key.keysym.sym == SDLK_RETURN) {
                    if (plasmaconsole.isTextInTypeBuffer()) {
                        plasmaconsole.isHighlighted = false;
                        ProcessConsoleCommand(plasmaconsole.enter().c_str());
                    }
                }
                if (event.key.keysym.sym > 31 && event.key.keysym.sym < 159) {
                    if (plasmaconsole.isHighlighted == false && event.key.keysym.sym != SDLK_x && event.key.keysym.sym != SDLK_z) {
                        currentPlayer.SetStill();
                        plasmaconsole.isHighlighted = true;
                    }
                    if (plasmaconsole.isHighlighted) {
                        plasmaconsole.addCharToTypeBuffer(char(event.key.keysym.unicode));
                    }
                }
            }
            break;
        }
        if (event.type == SDL_VIDEORESIZE) {
            window->window_w = event.resize.w;
            window->window_h = event.resize.h;
            window->resize();
            break;
        }
        if (event.type == SDL_QUIT) {
            window->quit(0);
            break;
        }
    }
}

void MainEngine::MotionHandler() {
//currently hooked right up with the camera for first person
    if (currentPlayer.isMovingForward) {
        if (currentPlayer.isRun) {
            cam.moveLocalZ(1.0f);
        }
        else {
            cam.moveLocalZ(0.45f);
        }
    }
    if (currentPlayer.isMovingBackward) {
        cam.moveLocalZ(-0.45f);
    }
    if (currentPlayer.isTurningRight) {
        cam.angle += 0.025f;
        cam.turn();
    }
    if (currentPlayer.isTurningLeft) {
        cam.angle -= 0.025f;
        cam.turn();
    }
    if (currentPlayer.isMovingUp) {
        cam.moveLocalY(0.45f);
    }
    if (currentPlayer.isMovingDown) {
        cam.moveLocalY(-0.45f);
    }
}

void MainEngine::startGUI(int window_w,int window_h) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0,window_w, 0, window_h);
    glScalef(1, -1, 1);
    glTranslatef(0, -window_h, 0);
    glMatrixMode(GL_MODELVIEW);
}


void MainEngine::endGUI() {
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}



void MainEngine::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
//	std::cout << SDL_GetTicks() << std::endl;
    glEnable(GL_DEPTH_TEST);

    prp_engine.draw(cam);
    MotionHandler();
    cam.update();
//start GUI stuff
    glPushMatrix();
    startGUI(window->window_w,window->window_h);
    glLoadIdentity();
    plasmaconsole.draw(window->window_w,window->window_h);
    endGUI();
    glPopMatrix();
//end GUI stuff
    SDL_GL_SwapBuffers();
}
