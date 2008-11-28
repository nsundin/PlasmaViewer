#include "EngineMessage.h"

EngineMessage::EngineMessage() { }

EngineMessage::~EngineMessage() { }

void EngineMessage::PrintVars() {
	printf("[msg]\n");
	printf("\ttype:'%i'\n",MsgType);
	printf("\tuseage:'%i'\n",MsgUseage);
	printf("\tchar01:'%s'\n",chars01);
	printf("\tchar02:'%s'\n",chars02);
	printf("\tchar03:'%s'\n",chars03);
	printf("\tint01:'%i'\n",int01);
	printf("\tint02:'%i'\n",int02);
	printf("\tint03:'%i'\n",int03);
	printf("\tpointer01:'%i'\n",pointer01);
	printf("\tpointer02:'%i'\n",pointer02);
	printf("\tpointer03:'%i'\n",pointer03);
	printf("[\\msg]\n");
}
