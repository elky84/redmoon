#include "ConnectorInterface.h"

namespace Redmoon
{

/**
@brief 생성자. 아무일도 안함.
*/
CConnectorInterface::CConnectorInterface() : m_bRefreshing(FALSE)
{
}

/**
@brief 소멸자. 아무일도 안함.
*/
CConnectorInterface::~CConnectorInterface()
{

}

/**
@brief 접속 시도 하는 메소드.
@param Socket 접속을 시도하는데에 이용되는 소켓
@param socketaddrin 접속을 시도하는 대상 구조체 포인터
@return 성공 여부
*/
bool CConnectorInterface::Connect(SOCKET Socket, SOCKADDR_IN* sockaddrin)
{
	if(isRefreshing())
		return false;

	int nRet = connect(Socket, (LPSOCKADDR)sockaddrin, sizeof( SOCKADDR_IN ));
	if (nRet == SOCKET_ERROR) 
	{
		if(WSAEWOULDBLOCK == WSAGetLastError())
		{
			LOG_INFO(_T("%s WSAWOULDBLOCK"), __TFUNCTION__);
			return true;
		}

		LOG_INFO(_T("connect failed : %d, %s, %d"), WSAGetLastError(), inet_ntoa(sockaddrin->sin_addr),  ntohs(sockaddrin->sin_port));
		return false;
	}
	return true;
}

/**
@brief 소켓 재사용 대기 상태인지 여부
@param bRefreshing 재사용 대기 상태
*/
void CConnectorInterface::SetRefreshing(BOOL bRefreshing)
{
	InterlockedExchange((LONG *)(&m_bRefreshing), (LONG)bRefreshing);
}

} //namespace Redmoon