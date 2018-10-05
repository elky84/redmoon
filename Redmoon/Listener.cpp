#include "Listener.h"
#include "SessionManager.h"
#include "Session.h"
#include <MSWSock.h>
#include "PeerManager.h"
#include "IocpThreadManager.h"
#include "PacketRegister.h"

namespace Redmoon
{

HANDLE CListener::m_hIOCP = INVALID_HANDLE_VALUE;
int CListener::m_nRefCount = 0;

/**
@brief 생성자. 
@param pFunc Peer생성 Allocate함수 포인터.
*/
CListener::CListener(ALLOCATE_FUNCTION_PTR(pFunc)) : m_pFunc(pFunc), m_nAcceptSessionCount(0), m_nMaxSessionCount(0)
{
	m_nRefCount++;
	m_pcPeerManager.Reset(new CPeerManager);
	m_pcAcceptor.Reset(new CAcceptor(this));
}

/**
@brief 소멸자. Listener가 모두 사라지면 Worker스레드를 없앤다.
*/
CListener::~CListener()
{
	m_pcAcceptor.Reset();
	if(--m_nRefCount == 0)
	{
		FreeThread();
	}

	for(STL_MAP_SESSION::iterator it = m_stl_map_Session.begin(); it != m_stl_map_Session.end(); ++it) //모든 세션을 클리어 해준다.
	{
		CSession* pcSession = it->second;
		CSessionManager::InstancePtr()->Push(pcSession);
	}
}

/**
@brief Listen을 시작하는 메소드.
@param nPort Listen할 포트
@param nWaitingQueueSize 접속 가능한 유저 카운트.
@return 성공 여부.
*/
bool CListener::Start(int nPort, int nWaitingQueueSize)
{
	if(m_hIOCP == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR(_T("%s Fail. m_hIOCP == INVALID_HANDLE_VALUE. You have to call AllocThread Method."), __TFUNCTION__);
		return false;
	}

	if(!Bind(nPort))
	{
		LOG_ERROR(_T("%s Bind() Fail"), __TFUNCTION__);
		return false;
	}

	if(!Listen(nWaitingQueueSize))
	{
		LOG_ERROR(_T("%s Listen() Fail"), __TFUNCTION__);
		return false;
	}

	HANDLE hIOCP = CreateIoCompletionPort((HANDLE)m_Socket, m_hIOCP, 0, 0);
	if(hIOCP == NULL)
	{
		LOG_ERROR(_T("%s CreateIoCompletionPort() hIOCP Fail."), __TFUNCTION__);
		return false;
	}

	CSessionManager::InstancePtr()->New(nWaitingQueueSize);
	for(int i = 0; i < nWaitingQueueSize; i++)
	{
		CSession* pcSession = CSessionManager::InstancePtr()->Pop();
		if(!_Accept(pcSession))
		{
			LOG_ERROR(_T("%s Accept() Fail"), __TFUNCTION__);
			return false;
		}

		CPeer*pcPeer = NULL;
		if(m_pFunc)
		{
			pcPeer = m_pFunc(pcSession->GetIdx());
		}
		else
		{
			pcPeer = _Allocate(pcSession->GetIdx());
		}
		pcSession->SetPeer(pcPeer);
		m_stl_map_Session.insert(STL_MAP_SESSION::value_type(pcSession->GetIdx(), pcSession));
	}

	m_nMaxSessionCount = nWaitingQueueSize;
	LOG_ALARM(_T("%s Success [%s:%d] [%d]"), __TFUNCTION__, GetIP().c_str(), nPort, nWaitingQueueSize);
	return true;
}

/**
@brief AliveCheck해서, 오랫동안 패킷이 오지 않은 CSession를 List에 담아주는 메소드
@param listDisconnectedSession 오랫동안 패킷이 오지 않은 CSession를 담을 리스트
*/
void CListener::_AliveCheck(std::list<CSession*>&listDisconnectedSession)
{
	DWORD curtick = GetTickLimit49Day();
	_Enter();
	for(STL_MAP_SESSION::iterator it = m_stl_map_UsingSession.begin(); it != m_stl_map_UsingSession.end(); ++it)
	{
		CSession* pcSession = it->second;
		if(pcSession->isConnected() && !pcSession->CheckTick(curtick))
		{
			LOG_DEBUG(_T("%s CheckTick() False [%d]"), __TFUNCTION__, pcSession->GetIdx());
			listDisconnectedSession.push_back(pcSession);
		}
	}
	_Leave();
}

/**
@brief AliveCheck해서, 오랫동안 패킷이 오지않은 CSession의 접속을 끊는다.
*/
void CListener::AliveCheck()
{
	std::list<CSession*> listDisconnectedSession;
	_AliveCheck(listDisconnectedSession);

	while(false == listDisconnectedSession.empty())
	{
		CSession* pcSession = listDisconnectedSession.back();
		listDisconnectedSession.pop_back();

		Disconnect(pcSession);
	}
}

/**
@brief 접속중인 Peer들의 Act를 호출해준다.
*/
void CListener::Act()
{
	GetPeerManager()->Act();
}

/**
@brief 접속중인 Peer들에게 패킷을 브로드 캐스트한다.
@param nLen pData의 크기
@param pData 기록할 데이터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@desc pData가 버퍼의 앞쪽, pAddData가 pData의 뒤쪽에 기록된다.
*/
void CListener::BroadCast(int nLen, void* pData, int nAddLen /* = 0 */, void* pAddData /* = 0 */)
{
	GetPeerManager()->BroadCast(nLen, pData, nAddLen, pAddData);
}

/**
@brief 접속 완료 시 불려진다.
@param pcSession 접속이 완료된 CSession
*/
void CListener::OnConnected(CSession* pcSession)
{
	GetPeerManager()->Set(pcSession->GetIdx(), pcSession->GetPeer());
	pcSession->Initialize();

	if(setsockopt(pcSession->GetSocket(), SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char* )&m_Socket, sizeof(SOCKET)))
	{
		LOG_DEBUG(_T("%s setsockopt SO_UPDATE_ACCEPT_CONTEXT fail: %d [%d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx());
	}

	int localLen, remoteLen;
	sockaddr_in* pLocal,* pRemote;
	GetAcceptExSockaddrs(pcSession->GetConnectIocpContext()->GetHeadBuffer(), 0, ACCEPT_ADDRESS_LENGTH, ACCEPT_ADDRESS_LENGTH, (sockaddr** )&pLocal, &localLen, (sockaddr** )&pRemote, &remoteLen);
	pcSession->SetSockAddr(pRemote);

	_Enter();
	m_stl_map_UsingSession.insert(STL_MAP_SESSION::value_type(pcSession->GetIdx(), pcSession));
	_Leave();

	InterlockedDecrement(&m_nAcceptSessionCount);

	HANDLE hIocp = CreateIoCompletionPort((HANDLE)pcSession->GetSocket(), m_hIOCP, (ULONG_PTR)pcSession, 0); //억셉트된 소켓을 IOCP에 참여
	if(INVALID_HANDLE_VALUE == hIocp)
	{
		LOG_DEBUG(_T("%s CreateIoCompletionPort fail: %d [%d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx());
	}

	pcSession->SetConnected(TRUE);

	LOG_DEBUG(_T("%s Success [%d]"), __TFUNCTION__, pcSession->GetIdx());
}

/**
@brief 접속 끊기 메소드
@param pcSession 접속 끊기 시도하는 CSession
*/
void CListener::Disconnect(CSession* pcSession)
{
	if(pcSession == NULL)
		return;

	if(pcSession->isConnected())
	{
		LOG_INFO(_T("%s [%d]"), __TFUNCTION__, pcSession->GetIdx());
		CPacketRegister::Register(PACKETEVENT_DISCONNECT, pcSession);
	}
}

/**
@brief 접속 끊기 메소드
@param nIdx 접속 끊기 시도하는 CSocketConext의 인덱스
*/
void CListener::Disconnect(int nIdx)
{
	Disconnect(CSessionManager::InstancePtr()->Get(nIdx));
}

/**
@brief Io실패시 불려지는 메소드
@param pcSession Io가 실패한 CSession
*/
void CListener::OnIoFailed(CSession* pcSession)
{
	Disconnect(pcSession);
}
	
	
/**
@brief 접속이 끊겼을 시 불려진다.
@param pcSession 접속이 끊긴 CSession
*/
void CListener::OnDisconnect(CSession* pcSession)
{
	if(pcSession->isConnected())
	{
		pcSession->DisconnectProcess();

		GetPeerManager()->Release(pcSession->GetIdx()); //빼는건 미리 빼줌.

		_Enter();
		m_stl_map_UsingSession.erase(pcSession->GetIdx());
		_Leave();
	}
}

	
/**
@brief 소켓 재사용 가능시
@param pcSession CSession
*/
void CListener::OnReuse(CSession* pcSession)
{
	_Accept(pcSession);
}

/**
@brief접속/억셉트 실패시
@param pcSession CSession 포인터
*/
void CListener::ConnectFailed(CSession* pcSession)
{
	_Accept(pcSession);
}

/**
@brief 소켓 재사용 요청시 호출하는 메소드
@param pcSession CSession
*/
void CListener::Reuse(CSession* pcSession)
{
	if(m_nAcceptSessionCount < min(m_nMaxSessionCount / 10, m_nMaxSessionCount))
	{
		pcSession->CloseSocket();
		pcSession->OnReuse();
		return;
	}

	struct linger stLinger = {1, 0};
	if(SOCKET_ERROR == setsockopt(pcSession->GetSocket(), SOL_SOCKET, SO_LINGER, (char* )&stLinger, sizeof(stLinger)))
	{
		LOG_ERROR(_T("%s setsockopt() SO_LINGER Fail [%d] [%d] [%s:%d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx(), pcSession->GetIP().c_str(), pcSession->GetPort());
		pcSession->CloseSocket();
		pcSession->OnReuse();
		return;
	}

	if(SOCKET_ERROR == ::shutdown(pcSession->GetSocket(), SD_BOTH))
	{
		LOG_ERROR(_T("%s shutdown() Fail [%d] [%d] [%s:%d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx(), pcSession->GetIP().c_str(), pcSession->GetPort());
		pcSession->CloseSocket();
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
			LOG_ERROR(_T("%s TransmitFile() Fail [%d] [%d] [%s:%d]"), __TFUNCTION__, WSAGetLastError(), pcSession->GetIdx(), pcSession->GetIP().c_str(), pcSession->GetPort());
			pcSession->CloseSocket();
			pcSession->OnReuse();
			return;
		}
	}	
}

/**
@brief Listener가 사용할 Thread를 생성한다.
@param nThreadCnt 생성할 ThreadCount
*/
void CListener::AllocThread(int nThreadCnt)
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
void CListener::FreeThread()
{
	CIocpThreadManager::InstancePtr()->Unregister(m_hIOCP);
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
bool CListener::Send(int nIdx, int nLen, void* pData, int nAddLen /* = 0 */, void* pAddData /* = 0 */)
{
	CSession* pcSession = CSessionManager::InstancePtr()->Get(nIdx);
	if(pcSession == NULL)
		return false;

	return Send(pcSession, nLen, pData, nAddLen, pAddData);
}
/**
@brief 접속중인 Peer들에게 패킷을 브로드 캐스트한다.
@param pcSession Send에 사용할 CSession
@param pData 기록할 데이터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@return 성공 여부
@desc pData가 버퍼의 앞쪽, pAddData가 pData의 뒤쪽에 기록된다.
*/
bool CListener::Send(CSession* pcSession, int nLen, void* pData, int nAddLen /* = 0 */, void* pAddData /* = 0 */)
{
	if(false == pcSession->Send(nLen, pData, nAddLen, pAddData))
	{
		Disconnect(pcSession);
		return false;
	}  
	return true;
}

/**
@brief CPeerEx 할당 메소드. virtual로 선언되어서 재정의 가능하다.
*/
CPeerEx* CListener::_Allocate(const int& nIdx)
{
	return new CPeerEx(nIdx);
}

/**
@brief 임의의 Peer를 반환하는 메소드.
@return 랜덤으로 Peer를 반환한다. Peer가 존재하지 않았다면 NULL 리턴.
*/
CPeer* CListener::GetRandPeer()
{
	return GetPeerManager()->GetRandPeer();
}

/**
@brief Accept 처리를 하고 Accept 걸린 갯수를 알아내는 메소드
@param pcSession CSession 포인터
@return 성공 여부
*/
bool CListener::_Accept(CSession* pcSession)
{
	if(m_pcAcceptor.Get() == NULL)
		return false;

	if(m_pcAcceptor->Accept(pcSession))
	{
		InterlockedIncrement(&m_nAcceptSessionCount);
		return true;
	}
	return false;
}

} //namespace Redmoon