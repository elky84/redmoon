#pragma once

#include "Common.h"
#include "Singleton.h"
#include <list>
#include "Thread.h"
#include "FunctionPtr.h"
#include "CriticalSection.h"

namespace Redmoon
{

#define TIMER_TICK 20

/**
@brief 타이머의 콜백 정보
*/
class CTimerCallBack
{
	///최종 타이머 연산 시간
	DWORD					m_dwLastTick;

	///타이머 간격
	DWORD					m_dwInterval;
protected:
	///상속으로만 생성하기 위해서 protected로 지정
	CTimerCallBack(DWORD dwInterval, DWORD dwTick);

public:
	///소멸자
	~CTimerCallBack();

	///Interval단위로 불려지는 메소드
	void Run(DWORD dwTick);

	///주기적 동작 순수 가상 함수
	virtual void Act() = 0;

	/**
	@brief 동작할 Tick인지 여부
	@param dwTick 현재 틱
	@return 동작해야 할 때 true 반환
	*/
	inline bool CheckTick(DWORD dwTick){return dwTick - m_dwLastTick >= m_dwInterval;}

	/**
	@brief interval 값 반환 
	@return interval값
	*/
	inline DWORD GetInterval(){return m_dwInterval;}
};


/**
@brief 타이머 클래스입니다. 주기적으로 실행해야 될 객체를 등록하면 해당 시간마다 호출되게 됩니다.
*/
class CTimer : public CThread
{
	typedef std::list<CTimerCallBack*>				STL_LIST_TIMERCALLBACK;
	typedef std::list<CTimerCallBack*>::iterator	STL_LIST_TIMERCALLBACK_ITERATOR;

public:
	/// 생성자
	CTimer();
	/// 소멸자
	virtual ~CTimer();

	/// 특정 시간마다 수행 할 작업을 등록합니다. 
	bool Register(CTimerCallBack* pTimerCallBack);
	/// 작업을 해제합니다.
	bool Unregister(CTimerCallBack* pTimerCallBack);

	/// 실행 CThread.의 가상함수
	virtual void Act();

private:
	/**
	@brief 존재 여부 검사
	@param pFunctor 검색할 Functor
	@return 존배 여부
	*/
	bool _IsExist(CTimerCallBack* pTimerCallBack)
	{
		STL_LIST_TIMERCALLBACK_ITERATOR itr = m_stl_list_Timers.begin();
		for(; itr!=m_stl_list_Timers.end(); ++itr)
		{
			CTimerCallBack* pCheckTimerCallBack = *itr;
			if( pTimerCallBack == pCheckTimerCallBack )
				return true;
		}
		return false;
	}
private:
	///이벤트 객체
	CEvent m_cEvent;

	///이벤트의 리스트
	STL_LIST_TIMERCALLBACK	m_stl_list_Timers;

	///최종 수행 틱
	UINT32				m_nTick;

	///동기화 객체
	CCriticalSection	m_cs;

	///싱글턴으로 선언
	STATIC_SINGLETON_PTR(CTimer);

};

} //namespace Redmoon