#include "SocketCommon.h"
#include "UDPSocket.h"
#include "P2PCommon.h"

namespace Redmoon
{

/**
@brief 생성자. 소켓을 생성하고, Recv에 사용할 버퍼를 할당한다.
*/
CUDPSocket::CUDPSocket() : CSocketInterface()
{
	Socket();
	m_pcRecvBuffer.Reset(new CBuffer(MAX_CLIENT_PACKET_BUFFER_SIZE));
}

/**
@brief 소멸자. 아무것도 안함
*/
CUDPSocket::~CUDPSocket()
{

}

/**
@brief UDP 소켓 생성 메소드
@return 성공 여부
*/
bool CUDPSocket::Socket()
{
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(INVALID_SOCKET == m_Socket)
	{
		LOG_ERROR(_T("%s socket() error. [%d]"), __TFUNCTION__, WSAGetLastError());
		return false;
	}
	LOG_DEBUG(_T("%s Success. [%d]"), __TFUNCTION__, m_Socket);
	return true;
}

/**
@brief 특정 포트를 생성한 소켓에 바인드 하는 메소드.
@return 성공 여부.
@desc 최대 255개 까지, 포트를 증가 시켜가며 바인드를 시도한다.
*/
bool CUDPSocket::Bind(int nPort)
{
	char pHostName[64];
	gethostname(pHostName, sizeof(pHostName));
	HOSTENT* pHostent= gethostbyname(pHostName);
	if(pHostent == NULL)
	{
		LOG_ERROR(_T("%s gethostbyname() == NULL [%s]"), __TFUNCTION__, pHostName);
		return false;
	}

	char* ip = inet_ntoa(*(LPIN_ADDR)*pHostent->h_addr_list);
	for(int i = 0; i < UDP_BIND_TRY_COUNT; i++, nPort++)
	{
		m_SockAddrIn = GetSockAddr(ip, nPort);
		int nRet = bind(m_Socket, (sockaddr*)&m_SockAddrIn, sizeof(m_SockAddrIn));
		if(nRet < 0)
		{
			LOG_DEBUG(_T("%s bind() Failed [%d] [%s:%d] [%d]"), __TFUNCTION__, nRet, ip, nPort, WSAGetLastError());
			Socket(); //바인드 실패하면, 다시 Socket() 함수 호출 해주어야 함!
			continue;
		}
		LOG_INFO(_T("%s Success [%s:%d]"), __TFUNCTION__, ip, nPort);
		return true;
	}
	return false;
}

/**
@brief 데이터 송신시 이용하는 메소드
@param refSocket 송신에 사용할 소켓
@param sockaddrin 대상 주소 구조체
@param nLen 보낼 데이터 크기
@param pData 보낼 데이터 포인터
@return 성공 여부
*/
bool CUDPSocket::Send(const SOCKET &refSocket, SOCKADDR_IN &sockaddrin, int nLen, void* pData)
{
	return SOCKET_ERROR != sendto(refSocket, reinterpret_cast<const char*>(pData), nLen, 0, (sockaddr*)&sockaddrin, sizeof(sockaddr));
}

/**
@brief 패킷 받으려 시도할때 부르는 메소드
@param sockaddrin 소켓 주소 구조체 포인터
@return 패킷을 받아서 데이터 담긴 CBuffer 포인터. (데이터가 꼬이거나 실패하면 NULL을 반환)
*/
CBuffer* CUDPSocket::Recv(sockaddr_in* sockaddrin)
{
	m_pcRecvBuffer->Init();
	int nSockaddrInSize = sizeof(sockaddr_in);

	//누구한테 온건지 알아야 되기 때문에, Receive 신호 오자마자 recvfrom한다.
	int nSize = recvfrom(m_Socket, m_pcRecvBuffer->GetTailBuffer(), m_pcRecvBuffer->GetRemainSize(), 0, (sockaddr*)sockaddrin, &nSockaddrInSize);
	if(SOCKET_ERROR == nSize)
	{
		LOG_INFO(_T("%s SOCKET_ERROR"), __TFUNCTION__);
		return NULL;
	}

	if(!m_pcRecvBuffer->JumpTail(nSize))
	{
		LOG_ERROR(_T("%s !m_pcRecvBuffer->JumpTail(%d)"), __TFUNCTION__, nSize);
		return NULL;
	}

	return m_pcRecvBuffer.Get();
}

} //namespace Redmoon