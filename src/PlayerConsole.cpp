#include "PlayerConsole.h"

void PlayerConsole::addLineToMainBuffer(std::string line) {
	scrolltxt.push_back(line);
}

void PlayerConsole::addCharToTypeBuffer(char c) {
	TypeBuffer.push_back(c);
}

std::string PlayerConsole::enter() {
	std::string ReturnBuffer = TypeBuffer;
	TypeBuffer.clear();
	return ReturnBuffer;
}

void PlayerConsole::draw() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//draw the TypeBuffer first
	texthandler->drawText((char*)TypeBuffer.c_str(),10,10);
}
