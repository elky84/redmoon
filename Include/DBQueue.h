#pragma once

#include "Common.h"
#include <queue>
#include "CriticalSection.h"
#include "Event.h"
#include "AutoPtr.h"

namespace Redmoon
{

/**
@brief DB패킷을 쌓아두는 Queue
*/
class CDBQueue
{
private:
	///DB패킷을 쌓아두는 큐의 typedef형
	typedef std::queue<char*> STL_QUEUE_DATA;

	///패킷이 왔을 때 쌓아두는 Queue
	STL_QUEUE_DATA* m_stl_queue_Producer;

	///쌓인 패킷을 가져와 계속 푸는 Queue
	STL_QUEUE_DATA* m_stl_queue_Consumer;

	//스레드 동작 여부
	bool m_bActive;

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;

private:
	///Producer와 Consumer를 교환하는 메소드
	void Swap();

protected:
	///Thread를 재웠다 깨웠다 하기 위한 이벤트
	CAutoPtr<CEvent> m_pcEvent;

public:
	///생성자
	CDBQueue(); 
	
	///소멸자
	virtual ~CDBQueue();

	/**
	@brief Consumer와 Producer가 둘다 비었는지 검사
	@return 비었는지 여부
	*/
	bool Empty(){return m_stl_queue_Consumer->empty() && m_stl_queue_Producer->empty();}

	/// 큐에 데이터를 넣는 메소드
	virtual void Push(int nSize, void *data);

	/// 큐에서 데이터를 하나 빼서 리턴한다.
	char* Pop();

	/**
	@brief 쓰레드를 멈추라는 메소드
	*/
	void Stop()
	{
		InterlockedExchange((LONG *)(&m_bActive), (LONG)false);
	}

	/**
	@brief 스레드가 활성화 중인지 여부를 반환하는 메소드
	@return 스레드가 활성화 중인지 여부
	*/
	inline bool isActive()
	{
		return m_bActive;
	}

private:
	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void _Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void _Leave(){m_CriticalSection.Leave();}
};

} //namespace Redmoon