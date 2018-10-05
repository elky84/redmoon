#include "stdafx.h"
#include "ServerPeer.h"
#include "PeerManager.h"
#include "LogManager.h"

CServerPeer::CServerPeer()
{
}

CServerPeer::~CServerPeer()
{
}

void CServerPeer::OnConnect()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);
}

void CServerPeer::OnDisconnect()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);
	Reuse();
}

void CServerPeer::OnReceive(int nLen, void* data)
{
	LOG_INFO(_T("%s [%d] [%s]"), __TFUNCTION__, nLen, data);
}

void CServerPeer::Act()
{
	static int i = 0;

	TCHAR buf[512] = "";
	SPRINTF(buf, _T("%s %d"), _T("Test"), i++);

	Send((int)_tcslen(buf) + 1, buf);
}