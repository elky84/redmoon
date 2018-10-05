#pragma once

#include "SocketCommon.h"

struct SPacketHeader
{
	Redmoon::PACKET_SIZE m_nSize;
	void* m_pData;
	SPacketHeader(Redmoon::PACKET_SIZE nSize, void* pData) : m_nSize(nSize), m_pData(pData)
	{

	}

	~SPacketHeader()
	{
		SAFE_DELETE_ARRAY(m_pData);
	}
};

class CPacketInterface
{
public:
	CPacketInterface();
	virtual ~CPacketInterface();

	virtual SPacketHeader GeneratePacket() = 0;
};