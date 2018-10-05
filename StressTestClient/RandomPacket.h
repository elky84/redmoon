#pragma once

#include "PacketInterface.h"

class CRandomPacket : public CPacketInterface
{
public:
	CRandomPacket();
	~CRandomPacket();

	virtual SPacketHeader GeneratePacket();
};