#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief 뮤텍스 동기화 객체 랩퍼
@desc 뮤텍스는 한 스레드만이 소유할 수 있다. (뮤텍스는 커널 동기화 객체 이므로 한 프로세스만이 소유 가능하단 의미도 됨)
*/
class CMutex
{
	///뮤텍스 핸들
	HANDLE m_hMutex;

public:
	/**
	@brief 생성자. 
	@param lpMutexAttributes 뮤텍스 속성
	@param bInitOwner 소유자 초기화
	@param szName 이름. 이름이 같은 뮤텍스끼리는 같은 핸들을 공유한다.
	*/
	CMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes = NULL, BOOL bInitOwner = FALSE, LPCTSTR szName = NULL)
	{
		m_hMutex = CreateMutex(lpMutexAttributes, bInitOwner, szName);
	}

	/**
	@brief 소멸자. 핸들 닫기
	*/
	~CMutex()
	{
		CloseHandle(m_hMutex);
	}

	/**
	@brief 해당 뮤텍스의 소유권을 요청한다.
	@return 결과값.
	*/
	DWORD Acquire()
	{
		return WaitForSingleObject(m_hMutex, INFINITE);
	}

	/**
	@brief 사용이 끝났다고 알린다.
	*/
	void Release()
	{
		ReleaseMutex(m_hMutex);
	};
};

} //namespace Redmoon