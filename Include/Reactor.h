#pragma once

#include <queue>
#include "CriticalSection.h"
#include "AutoPtr.h"
#include "Thread.h"
#include "Reactor.h"

namespace Redmoon
{

class CEvent;
class CPacket;

typedef std::queue<CPacket*> STL_QUEUE_PACKET;

/**
@brief 패킷 처리하는 워커 클래스
@desc 패킷을 처리하려면 쓰레드를 생성해주어야 한다. AllocThread호출시 파라미터로 넘어온 nThreadCount 만큼 스레드가 추가된다.
*/
class CReactor : public CThread, public CEvent, public CSingleton<CReactor>
{
public:
	///생성자
	CReactor();

	///소멸자
	virtual ~CReactor();

	///CReactor의 루프 메소드
	virtual void Act();

	///소멸 작업
	virtual void Release();

	///패킷 넣기
	void Push(CPacket* pcPacket);

private:

	///생산자, 소비자 스왑
	void _Swap();

	/**
	@brief 큐가 모두 비었는지 검사
	@return 큐가 모두 비었는지 여부
	*/
	bool _Empty(){return m_stl_queue_Consumer->empty() && m_stl_queue_Producer->empty();}

	///패킷 하나 꺼내기
	CPacket* _Pop();

private:
	///소비자 큐
	CAutoPtr<STL_QUEUE_PACKET> m_stl_queue_Consumer;

	///생산자 큐
	CAutoPtr<STL_QUEUE_PACKET> m_stl_queue_Producer;

	///소비자 락
	CCriticalSection m_ConsumerLock;

	///생산자 락
	CCriticalSection m_ProducerLock;

	///최종 Swap 시간
	DWORD m_dwLastSwapTick;
};

} //namespace Redmoon
