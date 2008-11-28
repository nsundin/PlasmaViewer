#ifndef __ENGINEMESSAGE_H__
#define __ENGINEMESSAGE_H__

#include <stdlib.h>
#include <stdio.h>

class EngineMessage {
public:
    enum MsgTypes {
        kLinkMgrLoad,
		kLinkMgrUnload,
    };
    enum Useage {
        kLocalOneUse,
		kGlobalOneUse,
		kLocalMultiUse,
		kGlobalMultiUse,
		kIgnore,
    };
	EngineMessage();
	~EngineMessage();
	void PrintVars();
	unsigned int MsgType;
	unsigned int MsgUseage;
	void* pointer01;
	void* pointer02;
	void* pointer03;
	char* chars01;
	char* chars02;
	char* chars03;
	int int01;
	int int02;
	int int03;
};

#endif