#pragma once

#include "SocketCommon.h"
#include "Thread.h"
#include "CriticalSection.h"
#include "ArrayUtil.h"

namespace Redmoon
{

/**
@brief Event를 스레드로 처리하는 쓰레드 클래스.
*/
template <typename T>
class CEventWorker : public CThread
{
	///EVENT 핸들 배열
	HANDLE m_aryEvent[MAXIMUM_WAIT_OBJECTS];

	///T형 객체 포인터를 담은 배열
	T* m_aryObject[MAXIMUM_WAIT_OBJECTS];

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;

	///배열 관리 유틸
	CArrayUtil<HANDLE> m_cArrayUtil;

public:
	
	/**
	@brief 생성자. 쓰레드를 생성하고, 각종 변수를 초기화한다.
	*/
	CEventWorker() : m_cArrayUtil(m_aryEvent, _countof(m_aryEvent), INVALID_HANDLE_VALUE)
	{
		Start();
	}

	/**
	@brief 소멸자. 쓰레드를 종료시킨다.
	*/
	virtual ~CEventWorker()
	{
		Stop();
	}

	/**
	@brief 이벤트 왔는지 검사하는 메소드
	@return 수행 결과.
	*/
	virtual void Act()
	{
		DWORD index = WSAWaitForMultipleEvents(m_cArrayUtil.GetUsingCount(), m_aryEvent, FALSE, INFINITE, FALSE);
		index = index - WSA_WAIT_EVENT_0;
		if(index == WSA_WAIT_FAILED)
			return;
		else if(!IS_VALID_VALUE(index, 0, MAXIMUM_WAIT_OBJECTS)) //배열 유효값 검사
			return;

		CScopeCriticalSection cs(m_CriticalSection);
		m_aryObject[index]->Proc();
	}

	/**
	@brief 이벤트 워커에 등록
	@param pcEventSelectInterface 이벤트 셀렉트 객체
	@return 성공 여부
	*/
	bool Register(T* pcObject)
	{
		CScopeCriticalSection cs(m_CriticalSection);
		ARRAYUTIL_INDEX nIndex = m_cArrayUtil.Add(pcObject->GetEvent());
		if(nIndex == -1)
			return false;

		m_aryObject[nIndex] = pcObject;
		return true;
	}

	/**
	@brief 이벤트 워커에서 해제
	@param pcEventSelectInterface 이벤트 셀렉트 객체
	@return 성공 여부
	*/
	bool Unregister(T* pcObject)
	{
		CScopeCriticalSection cs(m_CriticalSection);
		ARRAYUTIL_INDEX nIndex = m_cArrayUtil.Remove(pcObject->GetEvent());
		if(nIndex == -1)
			return false;
		
		m_aryObject[nIndex] = NULL;
		return true;
	}
};

} //namespace Redmoon