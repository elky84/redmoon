#include "Connector.h"
#include <MSWSock.h>
#include "Peer.h"
#include "IocpThreadManager.h"
#include "SessionManager.h"
#include "PacketRegister.h"

namespace Redmoon
{

HANDLE CConnector::m_hIOCP = INVALID_HANDLE_VALUE;
int CConnector::m_nRefCount = 0;

#pragma comment(lib,"mswsock.lib") 

/**
@brief 생성자
@param pcPeer IO를 담당할 피어를 생성자에서 전달 받는다.
@param bActive 시작과 동시에 바로 스타트 유효하게 동작 시킬껀지 여부.
*/
CConnector::CConnector(CPeer* pcPeer, BOOL bActive) : m_lpfnConnectEx(NULL), m_bConnecting(FALSE), m_bActive(bActive)
{
	m_nRefCount++;
	CSessionManager::InstancePtr()->New(1);
	m_pcSession = CSessionManager::InstancePtr()->Pop();

	m_pcPeer = pcPeer;	
	m_pcSession->SetPeer(pcPeer);
}

/**
@brief 소멸자. 각종 릴리즈 작업을 한다.
*/
CConnector::~CConnector()
{
	CSessionManager::InstancePtr()->Push(m_pcSession.Release()); //소유권 포기하고 큐에 넣음
	if(--m_nRefCount == 0)
	{
		FreeThread();
	}
}

/**
@brief 쓰레드 할당 메소드
@param nThreadCnt 이 파라미터 만큼 스레드를 생성한다.
*/
void CConnector::AllocThread(int nThreadCnt)
{
	if(m_hIOCP == INVALID_HANDLE_VALUE)
	{
		m_hIOCP = CreateIOCPHandle();
	}
	CIocpThreadManager::InstancePtr()->Register(m_hIOCP, nThreadCnt);
}

/**
@brief Listener가 사용하는 모든 쓰레드를 해제하는 메소드.
*/
void CConnector::FreeThread()
{
	CIocpThreadManager::InstancePtr()->Unregister(m_hIOCP);
}

/**
@brief 일정 시간 이상 패킷이 오지 않은 CSession를 Disconnect 해버린다.
*/
void CConnector::AliveCheck()
{
	DWORD curtick = GetTickLimit49Day();
	if(m_pcSession->isConnected() && false == m_pcSession->CheckTick(curtick))
	{
		LOG_INFO(_T("%s CheckTick() False [%d]"), __TFUNCTION__, GetIdx());
		Disconnect();
	}
}

/**
@brief 접속시에는 CPeer의 Act를, 접속중이 아닐때는 접속을 시도하는 메소드
*/
void CConnector::Act()
{
	if(FALSE == m_bActive)	
		return;

	if(TRUE == isConnected())
	{
		m_pcPeer->Act();
	}
	else
	{
		Connect();
	}
}

/**
@brief Disconnect을 시도하는 메소드.
*/
void CConnector::Disconnect()
{
	Disconnect(m_pcSession.Get());
}

/**
@brief Disconnect을 시도하는 메소드
@param pcSession 접속을 끊을 pcSocketConext
*/
void CConnector::Disconnect(CSession* pcSession)
{
	if(TRUE == pcSession->isConnected())
	{
		LOG_DEBUG(_T("%s [%d]"), __TFUNCTION__, GetIdx());
		CPacketRegister::Register(PACKETEVENT_DISCONNECT, pcSession);
	}
}

/**
@brief 소켓 재사용 가능시
@param pcSession CSession
*/
void CConnector::OnReuse(CSession* pcSession)
{
	SetRefreshing(FALSE);
	LOG_DEBUG(_T("%s [Err:%d] [%d] [%s:%d] [%d]"), __TFUNCTION__, WSAGetLastError(), m_pcSession->GetIdx(),m_pcSession->GetIP().c_str(), m_pcSession->GetPort(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
}

/**
@brief접속/억셉트 실패시
@param pcSession CSession 포인터
*/
void CConnector::ConnectFailed(CSession* pcSession)
{
	LOG_DEBUG(_T("%s [%d] [%d] [%s:%d] [%d] [%d, %d, %d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx(), pcSession->GetIP().c_str(), pcSession->GetPort(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
	InterlockedExchange((LONG *)(&m_bConnecting), (LONG)FALSE);
}

/**
@brief 소켓 재사용 요청시 호출하는 메소드
@param pcSession CSession
*/
void CConnector::Reuse(CSession* pcSession)
{
	LOG_DEBUG(_T("%s [Err:%d] [%d] [%s:%d] [%d] [%d, %d, %d]"), __TFUNCTION__, WSAGetLastError(), m_pcSession->GetIdx(),m_pcSession->GetIP().c_str(), m_pcSession->GetPort(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);

	struct linger stLinger = {1, 0};
	if(SOCKET_ERROR == setsockopt(pcSession->GetSocket(), SOL_SOCKET, SO_LINGER, (char* )&stLinger, sizeof(stLinger)))
	{
		LOG_ERROR(_T("%s setsockopt() SO_LINGER Fail [%d] [%d] [%s:%d] [%d] [%d, %d, %d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx(), pcSession->GetIP().c_str(), pcSession->GetPort(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
		_Set();
		pcSession->OnReuse();
		return;
	}

	if(SOCKET_ERROR == ::shutdown(pcSession->GetSocket(), SD_BOTH))
	{
		LOG_ERROR(_T("%s shutdown() Fail [%d] [%d] [%s:%d] [%d] [%d, %d, %d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx(), pcSession->GetIP().c_str(), pcSession->GetPort(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
		_Set();
		pcSession->OnReuse();
		return;
	}

	BOOL bRet = ::TransmitFile(pcSession->GetSocket(), NULL, 0, 0, pcSession->GetReuseIocpContext(), NULL, TF_DISCONNECT | TF_REUSE_SOCKET);
	if( TRUE == bRet)
	{     
		pcSession->OnReuse();
	}
	else
	{
		if(WSAGetLastError() != WSA_IO_PENDING) //WSA_IO_PENDING이 아닐 경우에는 진짜 실패. WSA_IO_PENDING은 다음 시그널이 도착한다.
		{
			LOG_ERROR(_T("%s TransmitFile() Fail [%d] [%d] [%s:%d] [%d] [%d, %d, %d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx(), pcSession->GetIP().c_str(), pcSession->GetPort(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
			_Set();
			pcSession->OnReuse();
			return;
		}
	}	
}

/**
@brief 접속이 끊겼을 시 불려진다.
@param pcSession 접속이 끊긴 CSession
*/
void CConnector::OnDisconnect(CSession* pcSession)
{
	if(TRUE == isConnected())
	{
		SetRefreshing(TRUE);
		pcSession->DisconnectProcess();
	}
}

/**
@brief소켓을 초기화하고 옵션을 설정하는 메소드
@return 성공 여부
*/
bool CConnector::_Set()
{
	if(m_pcSession->GetSocket() != INVALID_SOCKET)
	{
		m_pcSession->CloseSocket();
	}

	SOCKET hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(INVALID_SOCKET == hSocket)
	{
		LOG_ERROR(_T("%s WSASocket() Fail. [%d] [%d]"), __TFUNCTION__, WSAGetLastError(), GetIdx());
		return false;
	}
	m_pcSession->SetSocket(hSocket);

	DWORD dwBytes;
	GUID GuidConnectEx = WSAID_CONNECTEX;
	if (SOCKET_ERROR  == ::WSAIoctl( hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, sizeof(GuidConnectEx), &m_lpfnConnectEx, sizeof(m_lpfnConnectEx) , &dwBytes, NULL, NULL))
	{
		LOG_ERROR(_T("%s WSAIoctl() Fail [%d] [%d]"), __TFUNCTION__, WSAGetLastError(), GetIdx());
		m_pcSession->CloseSocket();
		return false;
	}

	int zero = 0;
	if( SOCKET_ERROR == setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (char*)&zero, sizeof(zero)) )
	{	  	   
		LOG_ERROR(_T("%s setsockopt() SO_SNDBUF error. [%d] [%d]"), __TFUNCTION__, WSAGetLastError(), GetIdx());
		m_pcSession->CloseSocket();
		return false;
	}

	if( SOCKET_ERROR == setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (char*)&zero, sizeof(zero)) )
	{	  	   
		LOG_ERROR(_T("%s setsockopt() SO_RCVBUF error. [%d] [%d]"), __TFUNCTION__, WSAGetLastError(), GetIdx());
		m_pcSession->CloseSocket();
		return false;
	}

	// 소켓주소구조체를 구성해서 bind작업을 한다. 주소구조체 구성시에는 Local주소중 하나를 사용하고, Protocol family만 지정해주면 된다.
	SOCKADDR_IN	localAddr;
	::ZeroMemory(&localAddr, sizeof(SOCKADDR_IN)); 
	localAddr.sin_family = AF_INET;   
	localAddr.sin_addr.s_addr = inet_addr(GetIP().c_str());	
	if( SOCKET_ERROR == ::bind( hSocket,( struct sockaddr * )&localAddr,sizeof(localAddr) ) )
	{
		LOG_ERROR(_T("%s bind() error. [%d]"), __TFUNCTION__, WSAGetLastError());
		m_pcSession->CloseSocket();
		return false;
	}

	HANDLE hIOCP = CreateIoCompletionPort((HANDLE)hSocket, m_hIOCP, (ULONG_PTR)m_pcSession.Get(), 0);
	if (INVALID_HANDLE_VALUE == hIOCP)
	{
		LOG_ERROR(_T("%s CreateIoCompletionPort Fail : %d, %d, %d [%d]"), __TFUNCTION__, WSAGetLastError(), m_pcSession->GetSocket(), m_hIOCP, GetIdx());
		m_pcSession->CloseSocket();
		return false;
	}

	m_pcSession->SetIocpInterface(this);
	return true;
}

/**
@brief 접속을 시도하는 메소드
@return 성공 여부
*/
bool CConnector::Start()
{ 
	if(m_hIOCP == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR(_T("%s Fail. m_hIOCP == INVALID_HANDLE_VALUE. You have to call AllocThread Method."), __TFUNCTION__);
		return false;
	}

	InterlockedExchange((LONG*)&m_bActive, (LONG)TRUE);
	return Connect();
}

/**
@brief 접속 대상의 ip와 port를 설정하는 메소드.
@param szIP 접속 대상의 IP
@param nPort 접속 대상의 Port
*/
bool CConnector::Set(const char* szIP, int nPort)
{
	m_pcSession->SetSockAddr(&GetSockAddr(szIP, nPort));
	return _Set();
}

/**
@brief 접속중인 Peer들에게 패킷을 브로드 캐스트한다.
@param nIdx 찾을 CSession의 인덱스
@param nAccKey 찾을 CSession의 누적 키 인덱스
@param nLen pData의 크기
@param pData 기록할 데이터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@return 성공 여부
@desc pData가 버퍼의 앞쪽, pAddData가 pData의 뒤쪽에 기록된다.
*/
bool CConnector::Send(const int nLen, const void* pData, const int nAddLen /* = 0 */, const void* pAddData /* = 0 */)
{
	if(TRUE == isConnected())
	{
		return m_pcPeer->Send(nLen, pData, nAddLen, pAddData);
	}
	return false;
}

/**
@brief 접속을 시도하는 메소드 (블러킹 함수)
@return 성공 여부
*/
bool CConnector::Connect()
{
	if(TRUE == isRefreshing() || TRUE == m_bConnecting)
		return false;

	InterlockedExchange((LONG *)(&m_bConnecting), (LONG)TRUE);
	BOOL bRet = m_lpfnConnectEx(m_pcSession->GetSocket(), (struct sockaddr*)m_pcSession->GetSockAddrIn(), sizeof(struct sockaddr), NULL, 0, NULL, m_pcSession->GetConnectIocpContext());
	if(FALSE == bRet && WSAGetLastError() != WSA_IO_PENDING)
	{
		LOG_ERROR(_T("%s m_lpfnConnectEx Fail: %d [%d] [%d, %d, %d]"), __TFUNCTION__, WSAGetLastError(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
		_Set();
		InterlockedExchange((LONG *)(&m_bConnecting), (LONG)FALSE);
		return false;
	}
	return true;
}

/**
@brief 접속 성공시 불려지는 메소드.
@param pcSession 접속 성공한 CSession
*/
void CConnector::OnConnected(CSession* pcSession)
{
	if(SOCKET_ERROR == setsockopt(pcSession->GetSocket(), SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0))
	{
		LOG_DEBUG(_T("%s setsockopt SO_UPDATE_CONNECT_CONTEXT fail: %d [%d] [%d, %d, %d]"), __TFUNCTION__, WSAGetLastError(), GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
	}

	pcSession->Initialize();
	pcSession->SetConnected(TRUE);
	InterlockedExchange((LONG *)(&m_bConnecting), (LONG)FALSE);
	LOG_DEBUG(_T("%s [%d] [%d, %d, %d]"), __TFUNCTION__, GetIdx(), isConnected(), isRefreshing(), m_bConnecting);
}

/**
@brief Io실패시 불려지는 메소드
@param pcSession Io가 실패한 CSession
*/
void CConnector::OnIoFailed(CSession* pcSession)
{
	Disconnect();
}

/**
@brief 커넥터를 멈추게 한다.
*/
void CConnector::Stop()
{
	if(TRUE == isConnected())
		Disconnect();

	InterlockedExchange((LONG*)&m_bActive, (LONG)FALSE);
}

} //namespace Redmoon