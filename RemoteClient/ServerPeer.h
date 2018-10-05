#pragma once

#include "Peer.h"

class CServerPeer : public Redmoon::CPeer
{
public:
	CServerPeer();
	~CServerPeer();

	void Act();

	virtual void OnConnect();
	virtual void OnDisconnect();

	virtual void OnReceive(int nLen, void* data);
};