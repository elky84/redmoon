#include "stdafx.h"
#include "ClientPeer.h"
#include "PeerManager.h"
#include "LogManager.h"

CClientPeer::CClientPeer()
{
}

CClientPeer::~CClientPeer()
{
}

void CClientPeer::OnConnect()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);
}

void CClientPeer::OnDisconnect()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);
	Reuse();
}

void CClientPeer::OnReceive(int nLen, void* data)
{
	LOG_INFO(_T("%s [%d] [%s]"), __TFUNCTION__, nLen, data);
	Send(nLen, data);
}

void CClientPeer::Act()
{
}