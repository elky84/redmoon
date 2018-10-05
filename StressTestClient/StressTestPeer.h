#pragma once

#include "Peer.h"
#include "Config.h"
#include "AutoPtr.h"

class CPacketInterface;
class CStressTestPeer : public Redmoon::CPeer
{
	Redmoon::CAutoPtr<CPacketInterface> m_pcPacketInterface;
	ACTION_TYPE &m_refAction;
public:
	CStressTestPeer(ACTION_TYPE & refAction);

	virtual void OnConnect();
	virtual void OnDisconnect();

	virtual void OnReceive(int nLen, void* data);

	virtual void Act();
};
