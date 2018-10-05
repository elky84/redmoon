#pragma once

#include "Macro.h"
#include "DBQueue.h"
#include "Thread.h"

namespace Redmoon
{

/**
@brief 디비 스레드 클래스. 큐 기능도 한다.
*/
template <typename T>
class CDBThread : public CThread, public CDBQueue
{
public:
	/**
	@brief 생성자. 스레드를 생성한다.
	*/
	CDBThread()
	{
		Start();
	}

	/**
	@brief 소멸자. 스레드를 멈춘다.
	*/
	virtual ~CDBThread()
	{
		Stop();
	}

	/**
	@brief 루프 메소드
	*/
	void Act()
	{
		while(m_pcEvent->Wait() != WAIT_TIMEOUT)
		{
			while(!Empty())
			{
				char *pData = Pop();
				if(pData)
				{
					Proc((T*)pData);
					SAFE_DELETE_ARRAY(pData);
				}
			}
		}
	}
	
	///데이터를 큐에 넣는 기능을 하는 메소드. 순수 가상 함수다.
	virtual void Push(T *data) = 0;

	///큐에 넣었던 것을 꺼내서 처리하는 기능을 하는 메소드. 순수 가상 함수다.
	virtual void Proc(T *data) = 0;

private:
	///쓰레드 핸들
	HANDLE m_hThread;
};

} //namespace Redmoon