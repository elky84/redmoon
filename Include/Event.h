#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief 이벤트 객체를 사용하기 편하게 랩핑한 클래스.
*/
class CEvent  
{
	///이벤트 핸들
	HANDLE m_hEvent;
public:
	/**
	@brief 생성자. 이벤트를 넘어온 파라미터에 따라 초기화한다.
	@param bManualReset 수동 초기화 할지 여부
	@param bInitialState 상태를 초기화 할지 여부
	@param lpEventAttributes 이벤트 속성
	@param lpName 이벤트 이름
	*/
	CEvent(BOOL bManualReset = FALSE, BOOL bInitialState = FALSE,LPSECURITY_ATTRIBUTES lpEventAttributes = NULL,  LPCTSTR lpName = NULL)
	{
		m_hEvent = CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
	}

	/**
	@brief 소멸자. 이벤트 핸들을 닫는다.
	*/
	~CEvent()
	{
		CloseHandle(m_hEvent);
	}

	/**
	@brief 이벤트를 초기화
	*/
	void Reset()
	{
		ResetEvent(m_hEvent);
	}

	/** 
	@brief 이벤트 깨우는 메소드@\
	@return 성공 여부
	*/
	BOOL Set()
	{
		return SetEvent(m_hEvent);
	}

	/**
	@brief 이벤트 대기 시키는 메소드
	@sec 대기 시킬 시간
	@return 결과 값
	*/
	DWORD Wait(DWORD sec=INFINITE)
	{
		return WaitForSingleObject(m_hEvent, sec);
	}
};

} //namespace Redmoon
