#include "stdafx.h"
#include "P2P.h"
#include "P2PPeerEx.h"
#include "P2PRelayConnector.h"

CP2P::CP2P()
{
	
}

CP2P::~CP2P()
{

}

Redmoon::CP2PPeer*  CP2P::AddPeer(int nIdx, SOCKADDR_IN* sockaddrin)
{
	Redmoon::CP2PPeer* peer = new CP2PPeerEx(m_Socket, nIdx, sockaddrin);
	return peer;	
}

void CP2P::RemovePeer(int nIdx)
{
	__super::RemovePeer(nIdx);
}

void CP2P::InitComplete()
{
	Redmoon::GetP2PRelayConnector()->Enter(1);
}