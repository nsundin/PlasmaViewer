#include "PlayerConsole.h"

void PlayerConsole::addLineToMainBuffer(std::string line) {
    scrolltxt.push_back(line);
}

void PlayerConsole::CursorMoveRight() {
    if (cursorInd < (TypeBuffer.length()))
        cursorInd +=1;
}

void PlayerConsole::CursorMoveLeft() {
    if (0 < cursorInd)
        cursorInd -=1;
}

void PlayerConsole::backSpace() {
    if (isTextInTypeBuffer() && cursorInd > 0) {
        TypeBuffer.erase(TypeBuffer.begin()+cursorInd-1);
        CursorMoveLeft();
    }
}

void PlayerConsole::addCharToTypeBuffer(char c) {
    TypeBuffer.insert(TypeBuffer.begin()+cursorInd,c);
    CursorMoveRight();
//	printf("%s\n",TypeBuffer.c_str());
}

void PlayerConsole::escape() {
    TypeBuffer.clear();
    cursorInd = 0;
    isHighlighted = false;
}

std::string PlayerConsole::enter() {
    ReturnBuffer = TypeBuffer;
    TypeBuffer.clear();
    cursorInd = 0;
    return ReturnBuffer;
}

bool PlayerConsole::isTextInTypeBuffer() {
    return (TypeBuffer.length() > 0);
}

void PlayerConsole::draw(int window_w,int window_h) {
    if (isHighlighted) {
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glColor4f(0.4f,0.4f,0.4f,0.5f);
        glBegin(GL_QUADS);
        glVertex2i(0,50);
        glVertex2i(window_w,50);
        glVertex2i(window_w,0);
        glVertex2i(0,0);
        glEnd();

        texthandler->drawText(">",99-texthandler->getHeight(),10);
        texthandler->drawText((char*)TypeBuffer.c_str(),100,10);
        int cursorpos = texthandler->getCharPos((char*)TypeBuffer.c_str(), 100, 10, cursorInd-1);

        if (cursorInd == 0 && cursorpos == 0)
            texthandler->drawText("|",(100-(texthandler->GetPipeWidth()/2)),10);
        else
            texthandler->drawText("|",(cursorpos+(texthandler->GetPipeWidth()/2)),10);
    }
}

