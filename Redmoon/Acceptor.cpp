#include "Acceptor.h"
#include "Session.h"
#include "Listener.h"
#include "SessionManager.h"
#include <MSWSock.h>

namespace Redmoon
{

/**
@brief 생성자. CListener의 포인터를 받아서 들고 있는다.
*/
CAcceptor::CAcceptor(CListener* pcListener) : m_pcListener(pcListener)
{
}

/**
@brief 소멸자. 별다른 일을 하지 않는다.
*/
CAcceptor::~CAcceptor()
{
	LOG_DEBUG(_T("%s Complete"), __TFUNCTION__);
}

/**
@brief Accept를 다수 거는 메소드.
@param pcSession 세션
@return 성공 여부
@desc 실질적인 Accept 동작이 이뤄지는 메소드.
*/
bool CAcceptor::Accept(CSession* pcSession)
{
	if(pcSession == NULL)
	{
		LOG_ERROR(_T("%s pcSession == NULL"), __TFUNCTION__);
		return false;
	}

	if(pcSession->GetSocket() == INVALID_SOCKET)
	{
		pcSession->SetSocket(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED));
		if(INVALID_SOCKET == pcSession->GetSocket())
		{
			LOG_ERROR(_T("%s WSASocket() error. [%d]"), __TFUNCTION__, WSAGetLastError());
			CSessionManager::InstancePtr()->Push(pcSession);
			return false;
		}

		int zero = 0;
		if( SOCKET_ERROR == setsockopt(pcSession->GetSocketRef(), SOL_SOCKET, SO_SNDBUF, (char*)&zero, sizeof(zero)) )
		{	  	   
			LOG_ERROR(_T("%s setsockopt() SO_SNDBUF error. [%d]"), __TFUNCTION__, WSAGetLastError());
			CSessionManager::InstancePtr()->Push(pcSession);
			return false;
		}

		if( SOCKET_ERROR == setsockopt(pcSession->GetSocketRef(), SOL_SOCKET, SO_RCVBUF, (char*)&zero, sizeof(zero)) )
		{	  	   
			LOG_ERROR(_T("%s setsockopt() SO_RCVBUF error. [%d]"), __TFUNCTION__, WSAGetLastError());
			CSessionManager::InstancePtr()->Push(pcSession);
			return false;
		}
	}

	pcSession->SetIocpInterface(m_pcListener.Get());

	CIocpContext* pAcceptContext = pcSession->GetConnectIocpContext();
	if(!::AcceptEx(m_pcListener->GetSocket(), pcSession->GetSocket(), pAcceptContext->GetHeadBuffer(), 0, ACCEPT_ADDRESS_LENGTH, ACCEPT_ADDRESS_LENGTH, NULL, pAcceptContext))
	{
		if(WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			LOG_ERROR(_T("%s error [%d]"), __TFUNCTION__, WSAGetLastError());
			CSessionManager::InstancePtr()->Push(pcSession);
			return false;
		}
	}
	LOG_DEBUG(_T("%s Success"), __TFUNCTION__);
	return true;
}

}