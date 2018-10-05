#pragma once

#include "Peer.h"
#include "Singleton.h"

class CTestPeer : public Redmoon::CPeer
{
public:
	CTestPeer();
	virtual ~CTestPeer();

	virtual void OnConnect();
	virtual void OnDisconnect();

	virtual void OnReceive(int nLen, void* data);

	virtual void Act();
};