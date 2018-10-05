#include "PipeContext.h"
#include "Pipe.h"

namespace Redmoon
{

/**
@brief 생성자. 파이프 포인터와 IO_TYPE을 전달 받는다.
@param pcPipe IO대상 파이프 포인터
@param eIoType IO의 종류
*/
CPipeContext::CPipeContext(CPipe* pcPipe, IO_TYPE eIoType) : m_pcPipe(pcPipe), m_eIoType(eIoType)
{
	this->hEvent = CreateEvent(FALSE, FALSE, NULL, NULL);
}

/**
@brief 소멸자.
이벤트 워커에서 등록 해제한다.
*/
CPipeContext::~CPipeContext()
{
	if(GetSmartSingleton()->Unregister(this))
	{
		//성공
	}
	SAFE_CLOSE_HANDLE(this->hEvent);
}

/**
@brief 이벤트 워커에 등록 요청한다.
*/
void CPipeContext::Register()
{
	GetSmartSingleton()->Register(this);
}

/**
@brief Event발생시 전달 받은 이벤트를 파이프에 전달한다.
*/
void CPipeContext::Proc()
{
	DWORD dwTransfer;
	BOOL bSuccess = GetOverlappedResult( 
		m_pcPipe->GetPipe(), // handle to pipe 
		this, // OVERLAPPED structure 
		&dwTransfer,            // bytes transferred 
		FALSE);            // do not wait 

	if(bSuccess == FALSE)
	{
		m_pcPipe->OnDisconnect();
		return;
	}

	if(bSuccess == TRUE && dwTransfer == 0)
	{
		if(GetIoType() == IOTYPE_CONNECT)
		{
			m_pcPipe->OnConnect();
			return;
		}
		else
		{
			m_pcPipe->OnDisconnect();
			return;

		}	
	}

	switch(GetIoType())
	{
	case IOTYPE_RECV:
		{
			m_pcPipe->OnReceive(dwTransfer, m_pcPipe->GetRecvHeadBuffer());
		}
		break;
	case IOTYPE_SEND:
		{
			m_pcPipe->OnSend(dwTransfer);
		}
		break;
	default: 
		{
			LOG_ERROR(_T("Invalid pipe state. %d"), GetIoType()); 
			m_pcPipe->Disconnect();
			return;
		}
	}
}

} //namespace Redmoon
