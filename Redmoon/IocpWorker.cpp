#include "IocpWorker.h"
#include "Session.h"
#include "PacketRegister.h"

namespace Redmoon
{

/**
@brief 생성자. 
@param hIOCP GQCS에서 사용할 IOCP핸들
*/
CIocpWorker::CIocpWorker(HANDLE hIOCP) : m_hIOCP(hIOCP)
{
	Start();
}

/**
@brief 소멸자. 스레드를 멈춘다.
*/
CIocpWorker::~CIocpWorker()
{
	if(INVALID_HANDLE_VALUE != m_hIOCP)
	{
		WaitForSingleObject(m_hIOCP, INFINITE);
	}
}

/**
@brief IOCP 루프 메소드
@desc GQCS와 여러 IO동작을 한다.
*/
void CIocpWorker::Act()
{
	BOOL bRet;
	DWORD dwTransfer;
	CSession* pcSession = NULL;
	CIocpContext* pIO = NULL;
	while(1)
	{
		bRet = GetQueuedCompletionStatus(m_hIOCP, &dwTransfer, (LPDWORD)&pcSession, (LPOVERLAPPED*)&pIO, INFINITE);
		if(pIO == NULL)
		{
			Stop();
			return;
		}

		if(!bRet || (bRet && dwTransfer == 0)) 
		{
			switch(pIO->GetIoType())
			{
			case IOTYPE_CONNECT:
				{
					LOG_DEBUG(_T("%s [%d] [%d] [%d] [%p, %d]"), __TFUNCTION__, WSAGetLastError(), bRet, dwTransfer, pIO, pIO->GetIoType());
					pcSession = pIO->GetSession();
					if(bRet)
					{
						CPacketRegister::Register(PACKETEVENT_CONNECT, pcSession);
					}
					else
					{
						pcSession->ConnectFailed(); //Try에 대한 실패이므로 큐에 넣을 필요 없음.
					}
				}
				break;
			case IOTYPE_REUSE:
				{
					pcSession->OnReuse();
				}
				break;
			case IOTYPE_SEND:
			case IOTYPE_RECV:
				{
					CPacketRegister::Register(PACKETEVENT_DISCONNECT, pcSession);
				}
				break;
			default:
				{
					CPacketRegister::Register(PACKETEVENT_DISCONNECT, pcSession);
					LOG_ERROR(_T("%s [%d] [IoType Not Defined: %d] [%d] [%d] [%p]"), __TFUNCTION__, WSAGetLastError(), pIO->GetIoType(), bRet, dwTransfer, pIO);
					break;

				}
				break;
			}
		}
		else //IO 성공
		{
			if(pIO->GetIoType() == IOTYPE_RECV)
			{
				pcSession->OnReceive(dwTransfer, pIO->GetHeadBuffer());
			}
			else if(pIO->GetIoType() == IOTYPE_SEND)
			{
				pcSession->OnSend(dwTransfer);
			}
			else
			{
				LOG_ERROR(_T("%s [%d] [IoType Not Defined: %d] [%d] [%d] [%p]"), __TFUNCTION__, WSAGetLastError(), pIO->GetIoType(), bRet, dwTransfer, pIO);
			}
		}
	}
}

/**
@brief 스레드를 멈춘다. 그리고 IOCP큐도 종료 메시지를 보낸다.
*/
void CIocpWorker::Stop()
{
	__super::Stop();
	PostQueuedCompletionStatus(m_hIOCP, 0, NULL, NULL);
}

} //namespace Redmoon
