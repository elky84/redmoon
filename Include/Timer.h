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
@brief Ÿ�̸��� �ݹ� ����
*/
class CTimerCallBack
{
	///���� Ÿ�̸� ���� �ð�
	DWORD					m_dwLastTick;

	///Ÿ�̸� ����
	DWORD					m_dwInterval;
protected:
	///������θ� �����ϱ� ���ؼ� protected�� ����
	CTimerCallBack(DWORD dwInterval, DWORD dwTick);

public:
	///�Ҹ���
	~CTimerCallBack();

	///Interval������ �ҷ����� �޼ҵ�
	void Run(DWORD dwTick);

	///�ֱ��� ���� ���� ���� �Լ�
	virtual void Act() = 0;

	/**
	@brief ������ Tick���� ����
	@param dwTick ���� ƽ
	@return �����ؾ� �� �� true ��ȯ
	*/
	inline bool CheckTick(DWORD dwTick){return dwTick - m_dwLastTick >= m_dwInterval;}

	/**
	@brief interval �� ��ȯ 
	@return interval��
	*/
	inline DWORD GetInterval(){return m_dwInterval;}
};


/**
@brief Ÿ�̸� Ŭ�����Դϴ�. �ֱ������� �����ؾ� �� ��ü�� ����ϸ� �ش� �ð����� ȣ��ǰ� �˴ϴ�.
*/
class CTimer : public CThread
{
	typedef std::list<CTimerCallBack*>				STL_LIST_TIMERCALLBACK;
	typedef std::list<CTimerCallBack*>::iterator	STL_LIST_TIMERCALLBACK_ITERATOR;

public:
	/// ������
	CTimer();
	/// �Ҹ���
	virtual ~CTimer();

	/// Ư�� �ð����� ���� �� �۾��� ����մϴ�. 
	bool Register(CTimerCallBack* pTimerCallBack);
	/// �۾��� �����մϴ�.
	bool Unregister(CTimerCallBack* pTimerCallBack);

	/// ���� CThread.�� �����Լ�
	virtual void Act();

private:
	/**
	@brief ���� ���� �˻�
	@param pFunctor �˻��� Functor
	@return ���� ����
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
	///�̺�Ʈ ��ü
	CEvent m_cEvent;

	///�̺�Ʈ�� ����Ʈ
	STL_LIST_TIMERCALLBACK	m_stl_list_Timers;

	///���� ���� ƽ
	UINT32				m_nTick;

	///����ȭ ��ü
	CCriticalSection	m_cs;

	///�̱������� ����
	STATIC_SINGLETON_PTR(CTimer);

};

} //namespace Redmoon