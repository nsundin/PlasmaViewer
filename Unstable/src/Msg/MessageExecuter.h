#ifndef __MESSAGEEXECUTER_H__
#define __MESSAGEEXECUTER_H__

#include "../Control/LinkManager.h"
#include "EngineMessage.h"
#include "../Draw/MainRenderer.h"

class MessageExecuter {
public:
	enum HandleStatus {
		kErrorUnknownType,
		kExecuted,
	};
	MainRenderer* renderer;
	LinkManager* lnkmgr;
	unsigned int HandleMessage(EngineMessage* msg);
};

#endif