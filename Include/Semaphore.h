#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief 세마포어 동기화 객체 랩퍼
@desc 세마포어는 자원을 분배해주는 커널 동기화 객체이다. 뮤텍스와 다른점은 여러 스레드에서 동시에 소유권을 가져갈 수 있다는 점이다.
*/
class CSemaphore
{
	///세마포어 핸들
	HANDLE m_hSemaphore;

public:
	/**
	@brief 생성자.
	@param lInitialCount 초기 자원 갯수. 이 값은 일반적으로 lMaximumCount와 같은 값을 입력한다.
	@param lMaximumCount 최대 자원 갯수.
	@param lpSemaphoreAttributes 세마포어 속성
	@param lpName 이름. 이름이 같은 세마포어끼리는 같은 핸들을 공유한다.
	*/
	CSemaphore(LONG lInitialCount, LONG lMaximumCount = lInitialCount, LPSECURITY_ATTRIBUTES lpSemaphoreAttributes = NULL, LPCTSTR lpName = NULL)
	{
		m_hSemaphore = CreateSemaphore(lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName);
	}

	/**
	@brief 소멸자. 세마포어 핸들을 닫는다.
	*/
	~CSemaphore()
	{
		CloseHandle(m_hSemaphore);
	}

	/**
	@brief 세마포어 사용권한을 얻을 수 있을때가지 대기한다. 
	@desc 대기 권한에서 리턴되어 사용권한을 가져가면서, 사용 가능 자원을 1 감소 시킨다.
	*/
	void Wait()
	{
		WaitForSingleObject(m_hSemaphore, INFINITE);
	}

	/**
	@brief 세마포어 자원의 사용이 끝났음을 알려준다.
	@param nReleaseCount 이 값 만큼의 자원 사용이 끝났음을 알려준다.
	*/
	void Release(int nReleaseCount = 1)
	{
		ReleaseSemaphore(m_hSemaphore, nReleaseCount, NULL);
	}
};

} //namespace Redmoon