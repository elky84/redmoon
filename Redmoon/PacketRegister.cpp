#include "PacketRegister.h"

namespace Redmoon
{

CPacketRegister::CPacketRegister()
{

}

CPacketRegister::~CPacketRegister()
{

}

void CPacketRegister::Register(const PACKET_EVENT& ePacketEvent, CSession* pcSession, CBuffer* pcBuffer /* = NULL */)
{
	CPacket* pcPacket = new CPacket(ePacketEvent, pcSession, pcBuffer);
	pcPacket->Register();
}

void CPacketRegister::Unregister(CPacket* pcPacket)
{
	SAFE_DELETE(pcPacket);
}

} //namespace Redmoon
