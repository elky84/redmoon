#pragma once

#include "Event.h"

namespace Redmoon
{
///Thread에 사용될 함수의 원형
inline DWORD WINAPI ThreadFunc(void* lpVoid);

/**
@brief Thread를 편리하게 사용하도록 랩핑한 클래스.
*/
class CThread
{
public:
	///소멸자. 쓰레드를 종료 한다.
	virtual ~CThread();

	/// Active상태 동안 Act메소드를 반복 실행한다.
	void Run();

	///스레드 릴리즈
	virtual void Release();

	/// 스레드 정지
	virtual void Stop();

	///스레드 시작
	void Start();

	///반복적으로 불려지는 메소드. 상속 받은 클래스에서 재정의해서 사용해야함.
	virtual void Act() = 0;

	/**
	@brief 스레드 시작시 불려지는 메소드. 재정의 가능하도록 가상 함수로 만들었다.
	@desc CreateThread로 불려지며 새로 생성된 스레드에서 불려진다.
	*/
	virtual void Begin()
	{
	}

	/**
	@brief 스레드 종료시 불려지는 메소드. 재정의 가능하도록 가상 함수로 만들었다.
	@desc CreateThread로 불려지며 새로 생성된 스레드에서 불려진다.
	*/
	virtual void End()
	{
		Release();
	}

	/// 스레드를 일시적으로 멈춘다.
	void Suspend();

	/// 스레드를 다시 구동 시킨다.
	void Resume();

	/**
	@brief 현재 스레드가 활동중인지를 반환하는 메소드
	@return 현재 스레드가 활동중인지 여부
	*/
	inline BOOL isActive()
	{
		if(m_hThread != INVALID_HANDLE_VALUE)
		{
			return TRUE;
		}
		return FALSE;
	}

protected:
 	/// 생성자. 쓰레드를 생성한다. 상속으로만 생성할 수 있도록 protected로 설정했다.
	CThread();

private:
	///쓰레드 핸들
	HANDLE m_hThread;

	///이벤트 객체
	CEvent m_cEvent;

	///스레드 ID
	DWORD m_dwThreadID;
};

} //namespace Redmoon