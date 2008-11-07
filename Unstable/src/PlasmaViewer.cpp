#include "MainEngine.h"
#include "prpengine.h"
SDLWindow* window  = new SDLWindow;
MainEngine* engine = new MainEngine;
plString base_program_path;

void drawLoading(float loaded, float loaded2) {
    const float inset = 48.0f;
    static float primary = 0.0, secondary = 0.0;
    if(loaded >= 0.0) primary = loaded;
    if(loaded2 >= 0.0) secondary = loaded2;
//    printf("drawLoading: %f/%f\n",primary,secondary);
    glViewport(0, 0, window->window_w, window->window_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (double)window->window_w, 0.0,(double)window->window_h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.27f,0.46f,0.33f);

    float fullbarwidth = window->window_w-inset*2.0-4.0; //foobarwidth anyone?
    float barwidth = fullbarwidth*primary;
    float barwidth2 = fullbarwidth*secondary;

    glRectf(inset+2.0f, 67.0f, inset+2.0+barwidth, 52.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(inset,68.0f);
    glVertex2f(window->window_w-inset-1.0,68.0f);
    glVertex2f(window->window_w-inset-1.0,50.0f);
    glVertex2f(inset,50.0f);
    glEnd();
    if(secondary > 0.0) {
        glRectf(inset+2.0f, 97.0f, inset+2.0+barwidth2, 82.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(inset,98.0f);
        glVertex2f(window->window_w-inset-1.0,98.0f);
        glVertex2f(window->window_w-inset-1.0,80.0f);
        glVertex2f(inset,80.0f);
        glEnd();
    }
    SDL_GL_SwapBuffers();
    engine->ProcessEvents();
}
void drawSecondaryProgress(float progress) {
    drawLoading(-1.0, progress);
}


int main(int argc, char** argv) {
	engine->window = window;

	window->init_SDL();
    engine->Init();

    base_program_path = (plString(argv[0]).beforeLast(PATHSEP)+PATHSEP);
    if (base_program_path.len() == 1) {
        base_program_path = "";
    }
    printf("%s\n",base_program_path.cstr());
    
    plString fontpath = base_program_path+ plString("FreeMono.ttf");
    
    printf("%s\n",fontpath.cstr());

    engine->plasmaconsole.texthandler = new DynText(fontpath.cstr(),18,1,1.0f,1.0f,1.0f,0.0f,0.0f,0.0f);
//    plDebug::Init(plDebug::kDLAll);

    //drawLoading(0.0f); //give 'em something to look at before the first page loads
    if (argc < (int) 2) {
        engine->lnkmgr.Load("ages\\Kveer.age");
        printf("can use prp or age-path as first argument, loading default\n");
//		return 0;
    }
    else {
        engine->lnkmgr.Load(argv[1]);
    }
    engine->prp_engine.UpdateList(false);

    plString fnipath = (plString(argv[1]).beforeLast('.')+plString(".fni"));
    engine->prp_engine.AttemptToSetFniSettings(fnipath);
    engine->prp_engine.AttemptToSetPlayerToLinkPointDefault(cam);

    window->resize();

    while(1) {
        engine->ProcessEvents();
        engine->draw();
    }
}
