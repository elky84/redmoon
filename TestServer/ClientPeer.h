#pragma once

#include "PeerEx.h"

class CClientPeer : public Redmoon::CPeerEx
{
public:
	CClientPeer(const int& nIdx);
	~CClientPeer();

	void Act();

	virtual int Verify(const unsigned int nLen, const void* pData);

	virtual void OnConnect();
	virtual void OnDisconnect();

	virtual void OnReceive(int nLen, void* data);
};