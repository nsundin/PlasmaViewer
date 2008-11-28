#include "MessageExecuter.h"

unsigned int MessageExecuter::HandleMessage(EngineMessage* msg) {
//	msg->PrintVars();
	switch (msg->MsgType) {
		case EngineMessage::kLinkMgrLoad:
			lnkmgr->Load((const char*)msg->chars01);
			break;
		default:
			printf("\ttype unknown\n");
			return kErrorUnknownType;
	}
	return kExecuted;
}

