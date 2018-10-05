#pragma once

#include "Peer.h"

class CClientPeer : public Redmoon::CPeer
{
public:
	CClientPeer();
	~CClientPeer();

	void Act();

	virtual void OnConnect();
	virtual void OnDisconnect();

	virtual void OnReceive(int nLen, void* data);
};