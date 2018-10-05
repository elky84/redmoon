#include "ConnectorEventSelect.h"
#include <stdlib.h>
#include "Buffer.h"
#include "Tick.h"

namespace Redmoon
{

/**
@brief 생성자. IO를 담당할 피어를 전달 받는다.
@param pcPeer IO를 담당할 피어
*/
CConnectorEventSelect::CConnectorEventSelect(CPeer* peer) : CEventSelectInterface(m_Socket), CIoInterface(peer)
{
	m_bConnect = false;
	m_pcTick.Reset(new CTick);
	m_pcRecvBuffer = new CBuffer(MAX_CLIENT_PACKET_BUFFER_SIZE);
	m_pcSendBuffer = new CBuffer(MAX_CLIENT_PACKET_BUFFER_SIZE);
}

/**
@brief 소멸자. 자원을 해제한다.
*/
CConnectorEventSelect::~CConnectorEventSelect()
{
	Enter();
	m_bConnect = false;

	SAFE_DELETE(m_pcRecvBuffer);
	SAFE_DELETE(m_pcSendBuffer);
	Leave();
}

/**
@brief 접속을 해제하는 메소드. 소켓을 닫는다.
@desc 이후 OnDisconnect 신호가 온다.
*/
void CConnectorEventSelect::Disconnect()
{
	CloseSocket();
}

/**
@brief 서버 접속 메소드
@param szIP 접속할 서버 IP
@param nPort 접속할 서버 포트
@return 접속 결과
*/
bool CConnectorEventSelect::Connect(const char* szIP, int nPort)
{
	if(m_bConnect)
	{
		LOG_INFO(_T("%s Already Connect Server"), __TFUNCTION__);
		Disconnect();
	}

	int value, length;
	length = sizeof(value);
	if(getsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&value ,&length) < 0)
	{
		LOG_ERROR(_T("%s getsockopt() SO_RCVBUF fail"), __TFUNCTION__);
		return false;
	}

	if(getsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&value ,&length) < 0)
	{
		LOG_ERROR(_T("%s getsockopt() SO_SNDBUF fail"), __TFUNCTION__);
		return false;
	}

	BOOL bNoDelay = TRUE;
	int nRet = ::setsockopt(m_Socket, IPPROTO_TCP, TCP_NODELAY, (const char FAR* )&bNoDelay, sizeof(bNoDelay));

	int size = MAX_CLIENT_PACKET_BUFFER_SIZE;
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizeof(size));
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizeof(size));

	m_SockAddrIn = GetSockAddr(szIP, nPort);
	if(!CConnectorInterface::Connect(m_Socket, &m_SockAddrIn))
		return false;

	m_pcPeer->SetSockAddrIn(&m_SockAddrIn);

	OnConnect();
	return true;
}

/**
@brief Send 완료 이벤트
@return 처리 결과
@desc connect 되고 난 후에 자동으로 한번 호출된다.
*/
bool CConnectorEventSelect::OnSend()
{
	int nRemainSendSize = m_pcSendBuffer->GetUsingSize();
	if (send(m_Socket, m_pcSendBuffer->GetHeadBuffer(), nRemainSendSize, 0) != SOCKET_ERROR)
	{
		m_pcSendBuffer->JumpHead(nRemainSendSize);
		m_pcSendBuffer->Compress();
		return true;
	}
	else 
	{
		if(GetLastError() == WSAEWOULDBLOCK) 
		{
			LOG_INFO(_T("%s GetLastError() == WSAEWOULDBLOCK"), __TFUNCTION__);
			return true;
		}

		Disconnect();
		return false;
	}
}
/**
@brief Receive 완료 이벤트
@return 처리 결과
@desc 내부적으로 피어의 PacketParse함수를 통해 패킷을 한개 단위로 쪼개고, 큐에 넣어둔다.
*/
bool CConnectorEventSelect::OnReceive()
{
	if(!isConnected())
		return false;

	unsigned int size = recv(m_Socket, m_pcRecvBuffer->GetTailBuffer(), m_pcRecvBuffer->GetRemainSize(), 0);
	if(SOCKET_ERROR == size) return false;
	if(!m_pcRecvBuffer->JumpTail(size)) return false;

	m_pcTick->ResetTick(); //패킷이 왔으면 어떤 패킷이든간에 오케이~
	while(0 < m_pcRecvBuffer->GetUsingSize())
	{
		int nPacketLen = m_pcPeer->Verify(m_pcRecvBuffer->GetUsingSize(), m_pcRecvBuffer->GetHeadBuffer());
		if(nPacketLen == 0)
			break;

		Push(new CBuffer(nPacketLen, m_pcRecvBuffer->GetHeadBuffer()));
		m_pcRecvBuffer->JumpHead(nPacketLen);
	}
	m_pcRecvBuffer->Compress();
	return true;
}

/**
@brief 패킷 처리 이벤트.
@param nLen Receive온 패킷 크기
@param pData Recieve온 패킷 데이터 포인터
@return 처리 결과
@desc 완성된 패킷이 온것이므로 처리한다.
*/
bool CConnectorEventSelect::OnReceive(unsigned int len, void* data)
{
	m_pcPeer->OnReceive(len, data); //패킷단위로 불려짐. 걍 피어꺼 읽어!
	return true;
}

/**
@brief 접속 완료 이벤트
*/
void CConnectorEventSelect::OnConnect()
{
	m_pcTick->ResetTick();
	m_bConnect = true;
	if(m_pcPeer.Get())
		m_pcPeer->OnConnect();

	LOG_INFO(_T("%s Success."), __TFUNCTION__);
	CEventSelectInterface::OnConnect();
}

/**
@brief 접속 해제 이벤트
*/
void CConnectorEventSelect::OnDisconnect()
{
	if(!isConnected())
		return;

	m_bConnect = false;
	SetRefreshing(TRUE);

	LOG_INFO(_T("%s, %d"), __TFUNCTION__, WSAGetLastError());
	if(m_pcPeer.Get())
		m_pcPeer->OnDisconnect();

	m_pcRecvBuffer->Init();
	m_pcSendBuffer->Init();

	CloseSocket();
	Socket();
	CEventSelectInterface::OnDisconnect();
}

/**
@brief 서버에서 일정 시간 내에 패킷이 도달했는지 확인하고, 도달하지 않았으면 접속을 해제한다.
*/
void CConnectorEventSelect::AliveCheck()
{
	if(!isConnected())
		return; 

	if(!m_pcTick->CheckTick(GetTickLimit49Day()))
	{
		LOG_INFO(_T("%s CheckTick() False"), __TFUNCTION__);
		Disconnect();
	}
}

/**
@brief 데이터를 서버로 전달한다.
@param nLen pData의 크기
@param pData 기록할 데이터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@return 전송 결과
*/
bool CConnectorEventSelect::Send(const unsigned int nLen, const void* pData, const unsigned int nAddLen /* = 0 */, const void* pAddData /* = 0 */)
{
	CScopeCriticalSection cs(GetCriticalSection());
	if(!isConnected())
		return false;

	if (m_pcSendBuffer->AddData(nLen, pData, nAddLen, pAddData))  //버퍼 뒤에 데이터를 추가하자
	{ 
		OnSend();	//OnSend를 직접 불러주어야만 버퍼에 있는 데이터를 전송한다
		return true;
	}
	else 
	{
		Disconnect();
		return false;
	}
}

/**
@brief 소켓 재사용 가능시 호출하는 메소드
*/
void CConnectorEventSelect::Reuse()
{
	SetRefreshing(FALSE);
}

} //namespace Redmoon