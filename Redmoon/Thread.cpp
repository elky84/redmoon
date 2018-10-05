#include "Thread.h"
#include "MiniDump.h"

namespace Redmoon
{
	
/**
@brief Thread함수. CThread 클래스의 Run 메소드를 호출해준다.
*/
inline DWORD WINAPI ThreadFunc(void* lpVoid)
{
	__try
	{
		CThread*pcThread = ((CThread*)lpVoid);
		pcThread->Run();
	}
	__except(CMiniDumper::Handler(GetExceptionInformation()))
	{

	}
	return true;
}

/**
@brief 생성자. 쓰레드를 생성한다. 상속으로만 생성할 수 있도록 protected로 설정했다.
*/
CThread::CThread() : m_hThread(INVALID_HANDLE_VALUE), m_dwThreadID(0)
{
	
}

/**
@brief 소멸자. 쓰레드를 종료 한다.
*/
CThread::~CThread()
{
	Release();
}

/**
@brief Active상태 동안 Act메소드를 반복 실행한다.
*/
void CThread::Run()
{
	Begin();
	while(m_cEvent.Wait(0) != WAIT_OBJECT_0)
	{
		Act();
	}
	End();
}

/**
@brief 스레드 릴리즈
*/
void CThread::Release()
{
	if(INVALID_HANDLE_VALUE != m_hThread)
	{
		if(m_dwThreadID != GetCurrentThreadId())
		{
			Stop();
			WaitForSingleObject(m_hThread, INFINITE);
		}
		SAFE_CLOSE_HANDLE(m_hThread);
	}
}

/**
@brief 스레드 정지
*/
void CThread::Stop()
{
	m_cEvent.Set();
}

/**
@brief 스레드 시작
*/
void CThread::Start()
{
	m_hThread = CreateThread(NULL, 0, ThreadFunc, this, 0, &m_dwThreadID);
}

/**
@brief 스레드를 일시적으로 멈춘다.
*/
void CThread::Suspend()
{
	::SuspendThread(m_hThread);
}

/**
@brief 스레드를 다시 구동 시킨다.
*/
void CThread::Resume()
{
	::ResumeThread(m_hThread);
}

} //namepsace Redmoon