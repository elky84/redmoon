#include "Timer.h"
#include "Tick.h"

namespace Redmoon
{

/**
@brief 생성자. 타이머에 등록한다.
@param dwInterval 구동 간격
@param dwTick 현재 틱
*/
CTimerCallBack::CTimerCallBack(DWORD dwInterval, DWORD dwTick) : m_dwInterval(dwInterval), m_dwLastTick(dwTick)
{
	CTimer::InstancePtr()->Register(this);
}

/**
@brief 소멸자. 타이머에서 제거한다.
*/
CTimerCallBack::~CTimerCallBack()
{
	CTimer::InstancePtr()->Unregister(this);
}

/**
@brief Interval 단위로 동작하는 메소드
@param dwTick 현재 틱
*/
void CTimerCallBack::Run(DWORD dwTick)
{
	m_dwLastTick = dwTick;
	Act();
}

/**
@brief 생성자. 아무 일도 하지 않는다.
*/
CTimer::CTimer() : m_nTick(GetTickLimit49Day())
{
	Start();
}

/**
@brief 소멸자. 각종 해제작업을 수행합니다.
*/
CTimer::~CTimer()
{
	m_cEvent.Set();

	CScopeCriticalSection lock(m_cs);
	STL_LIST_TIMERCALLBACK_ITERATOR itr = m_stl_list_Timers.begin();
	for(; itr!=m_stl_list_Timers.end(); ++itr)
	{
		CTimerCallBack* pTimerCallBack = *itr;
		SAFE_DELETE(pTimerCallBack);
	}
	m_stl_list_Timers.clear();
}

/**
@brief 특정 시간마다 수행 할 작업을 등록합니다. 
@param dwTick 작업을 수행할 Tick 단위
@param pTimerCallBack 수행할 작업에 대한 TIMER_CALLBACK_FUNCTOR 의 포인터
*/
bool CTimer::Register(CTimerCallBack* pTimerCallBack)
{
	CScopeCriticalSection lock(m_cs);
	if( _IsExist(pTimerCallBack) ) return false;

	m_stl_list_Timers.push_back(pTimerCallBack);
	return true;
}

/**
@brief 특정 시간마다 수행 할 작업을 등록합니다. CTask의 가상함수
@param pTimerCallBack 해제할 작업에 대한 TIMER_CALLBACK_FUNCTOR 의 포인터
*/
bool CTimer::Unregister(CTimerCallBack* pTimerCallBack)
{
	CScopeCriticalSection lock(m_cs);
	STL_LIST_TIMERCALLBACK_ITERATOR itr = m_stl_list_Timers.begin();
	for(; itr!=m_stl_list_Timers.end(); ++itr)
	{
		CTimerCallBack* pcCheckTimerCallBack = *itr;
		if( pTimerCallBack == pcCheckTimerCallBack )
		{
			SAFE_DELETE(pcCheckTimerCallBack);
			m_stl_list_Timers.erase(itr);
			return true;
		}
	}
	return false;
}

/**
@brief 지정된 작업 수행
*/
void CTimer::Act()
{
	m_cEvent.Wait(TIMER_TICK);

	CScopeCriticalSection lock(m_cs);
	m_nTick = GetTickLimit49Day();

	STL_LIST_TIMERCALLBACK_ITERATOR itr = m_stl_list_Timers.begin();
	for(; itr!= m_stl_list_Timers.end(); ++itr)
	{
		CTimerCallBack* pTimerCallBack = *itr;
		if(pTimerCallBack->CheckTick(m_nTick)) 
		{
			pTimerCallBack->Run(m_nTick);
		}
	}
	::Sleep(1);
}

} //namespace Redmoon