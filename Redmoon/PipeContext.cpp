#include "PipeContext.h"
#include "Pipe.h"

namespace Redmoon
{

/**
@brief ������. ������ �����Ϳ� IO_TYPE�� ���� �޴´�.
@param pcPipe IO��� ������ ������
@param eIoType IO�� ����
*/
CPipeContext::CPipeContext(CPipe* pcPipe, IO_TYPE eIoType) : m_pcPipe(pcPipe), m_eIoType(eIoType)
{
	this->hEvent = CreateEvent(FALSE, FALSE, NULL, NULL);
}

/**
@brief �Ҹ���.
�̺�Ʈ ��Ŀ���� ��� �����Ѵ�.
*/
CPipeContext::~CPipeContext()
{
	if(GetSmartSingleton()->Unregister(this))
	{
		//����
	}
	SAFE_CLOSE_HANDLE(this->hEvent);
}

/**
@brief �̺�Ʈ ��Ŀ�� ��� ��û�Ѵ�.
*/
void CPipeContext::Register()
{
	GetSmartSingleton()->Register(this);
}

/**
@brief Event�߻��� ���� ���� �̺�Ʈ�� �������� �����Ѵ�.
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
