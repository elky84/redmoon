#pragma once

#include "P2PEventSelect.h"
#include "Singleton.h"

class CP2P : public Redmoon::CP2PEventSelect
{

public:
	CP2P();
	~CP2P();

private:
	virtual Redmoon::CP2PPeer* AddPeer(int nIdx, SOCKADDR_IN* sockaddrin);
	virtual void RemovePeer(int nIdx);

	virtual void InitComplete();
};