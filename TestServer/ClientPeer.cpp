#include "stdafx.h"
#include "ClientPeer.h"
#include "PeerManager.h"
#include "LogWriter.h"

CClientPeer::CClientPeer(const int& nIdx) : CPeerEx(nIdx)
{
}

CClientPeer::~CClientPeer()
{
}

void CClientPeer::OnConnect()
{
	LOG_INFO(_T("%s [%d]"), __TFUNCTION__, GetIdx());
}

void CClientPeer::OnDisconnect()
{
	LOG_INFO(_T("%s [%d]"), __TFUNCTION__, GetIdx());
	Reuse();
}

void CClientPeer::OnReceive(int nLen, void* data)
{
	Send(nLen, data);
}

void CClientPeer::Act()
{
}

int CClientPeer::Verify(const unsigned int nLen, const void* pData)
{
	//return rand() % 2 == 1 ? nLen : -1;
	return nLen;
}