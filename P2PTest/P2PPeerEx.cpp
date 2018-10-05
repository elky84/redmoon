#include "stdafx.h"
#include "P2PPeerEx.h"
#include "P2PTestCommon.h"

CP2PPeerEx::CP2PPeerEx(const SOCKET &refSocket, int nIdx, SOCKADDR_IN* sockaddrin) : CP2PPeer(refSocket, nIdx, sockaddrin)
{
	CP2PPeerEx::Register(P2PCOMMAND_CHAT, &CP2PPeerEx::Chat);
}

void CP2PPeerEx::Proc(Redmoon::RUDPCommand* command)
{
	P2PHeader* header = (P2PHeader*)command;
	CP2PPeerEx::PFTYPE pf = Get(header->GetCommand());
	if(pf == NULL)
	{
		LOG_INFO("%s [%d] [%d, %d]", __FUNCTION__, GetIdx(), header->GetCommand(), header->GetLen());
		return;
	}

	Call(header, pf);
}

void CP2PPeerEx::Chat(P2PHeader* header)
{
	P2PChat* chat = (P2PChat* )header;
	LOG_INFO("%d : %s", GetIdx(), chat->GetStr());
}