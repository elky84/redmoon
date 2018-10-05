#pragma once

#include "P2PPeer.h"
#include "FunctionPtr.h"

struct P2PHeader;
class CP2PPeerEx : public Redmoon::CP2PPeer, public Redmoon::CFunctionPtr<CP2PPeerEx, P2PHeader>
{
public:
	CP2PPeerEx(const SOCKET &refSocket, int nIdx, SOCKADDR_IN* sockaddrin);
	virtual void Proc(Redmoon::RUDPCommand* command);

public:
	void Chat(P2PHeader* header);
};