#include "ListenInterface.h"

namespace Redmoon
{

/**
@brief 생성자. 아무일도 안함
*/
CListenInterface::CListenInterface() : CTCPSocket()
{

}

/**
@brief 소멸자. 아무일도 안함
*/
CListenInterface::~CListenInterface()
{

}

/**
@brief 해당 포트를 소켓과 바인드 하는 메소드
@param nPort 바인드 할 포트
@return 성공 여부
*/
bool CListenInterface::Bind(int nPort)
{
	m_SockAddrIn.sin_port = htons(nPort);
	m_SockAddrIn.sin_addr.s_addr = htonl(INADDR_ANY); 	//모든 접속을 받겠다

	int nRet = bind(m_Socket, (SOCKADDR*)&m_SockAddrIn, sizeof(SOCKADDR_IN));
	if(0 != nRet)
	{
		LOG_ERROR(_T("%s bind() error. [%d]"), __TFUNCTION__, WSAGetLastError());
		return false;
	}
	return true;
}

/**
@brief 바인드 된 소켓을 Listen하도록 시작하는 메소드
@param nWaitingQueue Listen을 하기 위해 미리 만들어두는 큐의 크기. (최대 동접수와 매칭시켜라)
@return 성공 여부
*/
bool CListenInterface::Listen(int nWaitingQueue)
{
	//BOOL on = TRUE;
	//if (setsockopt(m_Socket, SOL_SOCKET,  SO_CONDITIONAL_ACCEPT, (char* )&on, sizeof(on)))
	//	return false;

	int nRet = listen(m_Socket, nWaitingQueue);
	if(0 != nRet)
	{
		LOG_ERROR(_T("%s listen() error. [%d]"), __TFUNCTION__, WSAGetLastError());
		return false;
	}
	return true;
}

} //namespace Redmoon