#include "PlayerConsole.h"

void PlayerConsole::addLineToMainBuffer(std::string line) {
    scrolltxt.push_back(line);
}

void PlayerConsole::backSpace() {
    if (isTextInTypeBuffer())
        TypeBuffer.erase(TypeBuffer.end()-1);
}

void PlayerConsole::addCharToTypeBuffer(char c) {
    TypeBuffer.push_back(c);
//	printf("%s\n",TypeBuffer.c_str());
}

std::string PlayerConsole::enter() {
    ReturnBuffer = TypeBuffer;
    TypeBuffer.clear();
    return ReturnBuffer;
}

bool PlayerConsole::isTextInTypeBuffer() {
    return (TypeBuffer.length() > 0);
}

void PlayerConsole::draw(int window_w,int window_h) {
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (isHighlighted) {
		glColor4f(0.4f,0.4f,0.4f,0.5f);
		glBegin(GL_QUADS);
		glVertex2i(0,50);
		glVertex2i(window_w,50);
		glVertex2i(window_w,0);
		glVertex2i(0,0);
		glEnd();

		texthandler->drawText(">",99-texthandler->getHeight(),10);
	}
    //draw the TypeBuffer
    texthandler->drawText((char*)TypeBuffer.c_str(),100,10);
}
