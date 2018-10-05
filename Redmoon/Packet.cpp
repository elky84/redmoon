#include "Packet.h"
#include "Reactor.h"
#include "Buffer.h"
#include "Session.h"

namespace Redmoon
{

/**
@brief 생성자. 패킷을 
@param nIdx 소켓 식별값
@param nAccKey 소켓 누적 키 값
@param pcBuffer 데이터를 담아둔 버퍼
*/
CPacket::CPacket(const PACKET_EVENT& ePacketEvent, CSession* pcSession, CBuffer* pcBuffer) : m_ePacketEvent(ePacketEvent), m_pcSession(pcSession), m_pcBuffer(pcBuffer), m_dwCreationTick(GetTickLimit49Day())
{
}

/**
@brief 소멸자. 암것도 안함.
*/
CPacket::~CPacket()
{
}

/**
@brief 데이터를 패킷큐에 넣는다
*/
void CPacket::Register()
{
	CReactor::InstancePtr()->Push(this);
}

/**
@brief 데이터를 푼다.
*/
void CPacket::Proc()
{
	switch(m_ePacketEvent)
	{
	case PACKETEVENT_CONNECT:
		m_pcSession->OnConnect();
		break;
	case PACKETEVENT_DISCONNECT:
		m_pcSession->OnDisconnect();
		break;
	case PACKETEVENT_PACKET:
		m_pcSession->PacketProcess(m_pcBuffer.Get());
		break;
	}

	LOG_DEBUG(_T("%s ElapsedTick [%llu]"), __TFUNCTION__, GetElapsedTick(GetTickLimit49Day()));
}

} //namespace Redmoon
