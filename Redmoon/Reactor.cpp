#include "Common.h"
#include "Reactor.h"
#include "Packet.h"
#include "PacketRegister.h"
#include "Event.h"

namespace Redmoon
{

/**
@brief 생성자. 아무일도 안함.
*/
CReactor::CReactor() : m_stl_queue_Consumer(new STL_QUEUE_PACKET), m_stl_queue_Producer(new STL_QUEUE_PACKET), m_dwLastSwapTick(GetTickLimit49Day())
{
	Start();	
}

/**
@brief 소멸자. 아무일도 안함.
*/
CReactor::~CReactor()
{
	Release();
	while(!_Empty())
	{
		CPacketRegister::Unregister(_Pop());
	}
}

/**
@brief 루프 메소드
*/
void CReactor::Act()
{
	Wait();
	DWORD dwStartTick = GetTickLimit49Day();
	while(!_Empty())
	{
		CPacket* pcPacket = _Pop();
		pcPacket->Proc();
		CPacketRegister::Unregister(pcPacket);
	}

	DWORD dwElapsedTick = GetTickLimit49Day() - dwStartTick;
	LOG_DEBUG(_T("%s [%llu]"), __TFUNCTION__, dwElapsedTick);
}

/**
@brief 소멸 작업
*/
void CReactor::Release()
{
	Stop();
	Set();
	__super::Release();
}

/**
@brief 생산자와 소비자를 스왑한다.
@desc 소비자가 모든걸 소비(empty상태) 한 이후에 불러야한다. 그래야 순서가 어긋나지 않는다.
*/
void CReactor::_Swap()
{
	m_ProducerLock.Enter();
	m_ConsumerLock.Enter();
	LOG_DEBUG(_T("%s Consumer[%lu] Producer[%lu] ReSwapTick[%llu]"), __TFUNCTION__, m_stl_queue_Consumer->size(), m_stl_queue_Producer->size(), GetTickLimit49Day() - m_dwLastSwapTick);

	STL_QUEUE_PACKET *pTemp = m_stl_queue_Consumer.Release();
	m_stl_queue_Consumer.Reset(m_stl_queue_Producer.Release());

	m_ConsumerLock.Leave();

	m_stl_queue_Producer.Reset(pTemp);
	m_ProducerLock.Leave();

	m_dwLastSwapTick = GetTickLimit49Day();
}

/**
@brief 소비자 큐에서 하나 꺼낸다.
@return 꺼낸 패킷
*/
CPacket* CReactor::_Pop()
{
	m_ConsumerLock.Enter();
	if(m_stl_queue_Consumer->empty())
	{
		_Swap();
		if(m_stl_queue_Consumer->size()) //하나라도 있으면
		{
			m_ConsumerLock.Leave();
			return _Pop();
		}
		m_ConsumerLock.Leave();
		LOG_ERROR(_T("%s NULL"), __TFUNCTION__); //깨어났다면 무조건 하나는 있어야 되게끔 짜여져 있다.
		return NULL;
	}
	m_ConsumerLock.Leave();

	m_ConsumerLock.Enter();
	CPacket *pcPacket = m_stl_queue_Consumer->front();
	m_stl_queue_Consumer->pop();
	m_ConsumerLock.Leave();
	return pcPacket;
}

/**
@brief 도착한 패킷 큐에 쌓기
*/
void CReactor::Push(CPacket* pcPacket)
{
	m_ProducerLock.Enter();
	m_stl_queue_Producer->push(pcPacket);
	m_ProducerLock.Leave();
	Set();
}

} //namespace Redmoon