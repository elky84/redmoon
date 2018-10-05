#include "Thread.h"
#include "MiniDump.h"

namespace Redmoon
{
	
/**
@brief Thread�Լ�. CThread Ŭ������ Run �޼ҵ带 ȣ�����ش�.
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
@brief ������. �����带 �����Ѵ�. ������θ� ������ �� �ֵ��� protected�� �����ߴ�.
*/
CThread::CThread() : m_hThread(INVALID_HANDLE_VALUE), m_dwThreadID(0)
{
	
}

/**
@brief �Ҹ���. �����带 ���� �Ѵ�.
*/
CThread::~CThread()
{
	Release();
}

/**
@brief Active���� ���� Act�޼ҵ带 �ݺ� �����Ѵ�.
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
@brief ������ ������
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
@brief ������ ����
*/
void CThread::Stop()
{
	m_cEvent.Set();
}

/**
@brief ������ ����
*/
void CThread::Start()
{
	m_hThread = CreateThread(NULL, 0, ThreadFunc, this, 0, &m_dwThreadID);
}

/**
@brief �����带 �Ͻ������� �����.
*/
void CThread::Suspend()
{
	::SuspendThread(m_hThread);
}

/**
@brief �����带 �ٽ� ���� ��Ų��.
*/
void CThread::Resume()
{
	::ResumeThread(m_hThread);
}

} //namepsace Redmoon