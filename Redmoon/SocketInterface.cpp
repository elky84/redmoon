#include "SocketInterface.h"

namespace Redmoon
{

CWinSockObject CSocketInterface::m_WinSockObject;

/**
@brief 생성자. 소켓 인터페이스 초기화 작업을 한다
*/
CSocketInterface::CSocketInterface() : m_Socket(INVALID_SOCKET)
{
	m_SockAddrIn.sin_family = AF_INET;
	memset(m_SockAddrIn.sin_zero, 0, sizeof(m_SockAddrIn.sin_zero));
}

/*
@brief 소멸자. 릴리즈 메소드를 호출
*/
CSocketInterface::~CSocketInterface()
{
	//CloseSocket();
}

/**
@brief 소켓을 닫는 메소드
*/
bool CSocketInterface::CloseSocket()
{
	return CloseSocket(m_Socket);
}


/**
@brief 소켓을 닫는 메소드
@param refSocket 소켓의 참조자
*/
bool CSocketInterface::CloseSocket(SOCKET& refSocket)
{
	if(refSocket == INVALID_SOCKET)
	{
		LOG_INFO(_T("%s socket == INVALID_SOCKET."), __TFUNCTION__);
		return false;
	}

	struct linger stLinger = {1, 0};
	setsockopt(refSocket, SOL_SOCKET, SO_LINGER, (char* )&stLinger, sizeof(stLinger));

	::shutdown(m_Socket, SD_BOTH);

	if(0 != ::closesocket(refSocket))
	{
		LOG_ERROR(_T("%s ::closesocket() failed. [%d]"), __TFUNCTION__, WSAGetLastError());
		return false;
	}

	refSocket = INVALID_SOCKET;
	LOG_DEBUG(_T("%s Success."), __TFUNCTION__);
	return true;
}

} //namespace Redmoon
