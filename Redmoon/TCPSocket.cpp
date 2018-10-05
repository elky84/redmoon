#include "TCPSocket.h"

namespace Redmoon
{

/**
@brief 생성자. 소켓 생성 작업을 한다.
*/
CTCPSocket::CTCPSocket() : CSocketInterface()
{
	Socket();
}

/**
@brief 소멸자. 아무것도 안함.
*/
CTCPSocket::~CTCPSocket()
{

}

/**
@brief TCP 소켓을 생성한다.
@return 성공 여부
*/
bool CTCPSocket::Socket()
{
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(INVALID_SOCKET == m_Socket)
	{
		LOG_ERROR(_T("%s socket() error. [%d]"), __TFUNCTION__, WSAGetLastError());
		return false;
	}
	LOG_DEBUG(_T("%s Success. [%d]"), __TFUNCTION__, m_Socket);
	return true;
}

} //namespace Redmoon