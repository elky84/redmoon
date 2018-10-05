#pragma once

#include "Packet.h"

namespace Redmoon
{

class CPacketRegister
{
public:
	CPacketRegister();
	~CPacketRegister();

	static void Register(const PACKET_EVENT& ePacketEvent, CSession* pcSession, CBuffer* pcBuffer = NULL);
	static void Unregister(CPacket* pcPacket);
};

} //namespace Redmoon