#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief 크리티컬 섹션 객체. 사용하기 편하게 랩핑했다.
@desc 동기화 객체이긴하지만, 커널 동기화 객체가 아니고, 프로세스 내에서의 동기화 객체다.
*/
class CCriticalSection  
{
	///크리티컬 섹션 오브젝트
	CRITICAL_SECTION m_cs;

public:

	/**
	@brief 생성자. CRITICAL_SECTION 오브젝트를 초기화 한다.
	*/
	CCriticalSection()
	{
		::InitializeCriticalSection(&m_cs);
	}

	/**
	@brief 소멸자. CRITICAL_SECTION 오브젝트를 삭제 한다. 상속으로 사용하지 말라는 의미에서 가상 소멸자가 아니다.
	*/
	~CCriticalSection()
	{
		::DeleteCriticalSection(&m_cs);
	}

	/**
	@brief 사용 권한을 얻으려 요청한다.
	*/
	inline void Enter()	
	{
		::EnterCriticalSection(&m_cs);
	}

	/**
	@brief 사용이 끝났음을 알린다.
	*/
	inline void Leave()
	{
		::LeaveCriticalSection(&m_cs);
	}

#if(_WIN32_WINNT >= 0x0400)
	inline BOOL Try()
	{
		return TryEnterCriticalSection(&m_cs);
	}
#endif

};

/**
@brief 객체의 유효기간 동안 Enter하고, 객체가 소멸될때 Leave하는 유틸 클래스
*/
class CScopeCriticalSection
{
	///크리티컬 섹션 포인터
	CCriticalSection &m_cs;

public:
	/**
	@brief 생성자. 파라미터로 넘어온 CriticalSection을 Enter한다.
	@param cs 크리티컬 섹션 객체 포인터
	*/
	CScopeCriticalSection(CCriticalSection &cs) : m_cs(cs)
	{
		m_cs.Enter();
	}
	
	/**
	@brief 소멸자. 객체가 소멸되면서, Leave함으로 다른 객체가 사용권한을 가질 수 있도록 한다.
	*/
	~CScopeCriticalSection()
	{
		m_cs.Leave();
	}
};

} //namespace Redmoon
