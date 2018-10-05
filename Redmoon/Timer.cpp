#include "Timer.h"
#include "Tick.h"

namespace Redmoon
{

/**
@brief ������. Ÿ�̸ӿ� ����Ѵ�.
@param dwInterval ���� ����
@param dwTick ���� ƽ
*/
CTimerCallBack::CTimerCallBack(DWORD dwInterval, DWORD dwTick) : m_dwInterval(dwInterval), m_dwLastTick(dwTick)
{
	CTimer::InstancePtr()->Register(this);
}

/**
@brief �Ҹ���. Ÿ�̸ӿ��� �����Ѵ�.
*/
CTimerCallBack::~CTimerCallBack()
{
	CTimer::InstancePtr()->Unregister(this);
}

/**
@brief Interval ������ �����ϴ� �޼ҵ�
@param dwTick ���� ƽ
*/
void CTimerCallBack::Run(DWORD dwTick)
{
	m_dwLastTick = dwTick;
	Act();
}

/**
@brief ������. �ƹ� �ϵ� ���� �ʴ´�.
*/
CTimer::CTimer() : m_nTick(GetTickLimit49Day())
{
	Start();
}

/**
@brief �Ҹ���. ���� �����۾��� �����մϴ�.
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
@brief Ư�� �ð����� ���� �� �۾��� ����մϴ�. 
@param dwTick �۾��� ������ Tick ����
@param pTimerCallBack ������ �۾��� ���� TIMER_CALLBACK_FUNCTOR �� ������
*/
bool CTimer::Register(CTimerCallBack* pTimerCallBack)
{
	CScopeCriticalSection lock(m_cs);
	if( _IsExist(pTimerCallBack) ) return false;

	m_stl_list_Timers.push_back(pTimerCallBack);
	return true;
}

/**
@brief Ư�� �ð����� ���� �� �۾��� ����մϴ�. CTask�� �����Լ�
@param pTimerCallBack ������ �۾��� ���� TIMER_CALLBACK_FUNCTOR �� ������
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
@brief ������ �۾� ����
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