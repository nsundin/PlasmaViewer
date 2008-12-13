#include "MessageExecuter.h"

unsigned int MessageExecuter::HandleMessage(EngineMessage* msg) {
//	msg->PrintVars();
	switch (msg->MsgType) {
		case EngineMessage::kLinkMgrLoad:
			pthread_mutex_unlock(lnkmgr->pool->mutex);
			lnkmgr->Load((const char*)msg->chars01);
			pthread_mutex_lock(lnkmgr->pool->mutex);
			break;
		default:
			printf("\ttype unknown\n");
			return kErrorUnknownType;
	}
	return kExecuted;
}

