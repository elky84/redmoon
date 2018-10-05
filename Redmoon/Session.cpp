#include "Session.h"
#include "Peer.h"
#include "Tick.h"
#include <MSWSock.h> 
#include "PacketRegister.h"

#include "IocpInterface.h"
#include "MiniDump.h"

namespace Redmoon
{

/**
@brief 생성자. 각종 초기화 작업을 한다
@param nIdx 소켓 식별값. 상수로써 변하지 않는다.
@param pcIocpInterface IOCP기능을 해주는 CIocpInterface.
*/
CSession::CSession(const int nIdx, CIocpInterface* pcIocpInterface) : m_bConnected(FALSE), m_nIdx(nIdx), CIoInterface(NULL), m_pcIocpInterface(NULL), m_bSending(FALSE)
,m_pSendIocpContext(new CIocpContext(this, new CIocpEventSend(new CBuffer(MAX_SESSION_SEND_BUFFER_SIZE))))
,m_pRecvIocpContext(new CIocpContext(this, new CIocpEventRecv(new CBuffer(MAX_SESSION_RECV_BUFFER_SIZE))))
,m_pConnectIocpContext(new CIocpContext(this, new CIocpEventConnect))
,m_pReuseIocpContext(new CIocpContext(this, new CIocpEventReuse))
,m_pcTick(new CTick)
{
	SetIocpInterface(pcIocpInterface);
	memset(&m_SockAddrIn, 0, sizeof(m_SockAddrIn));
}

/**
@brief 소멸자. 
*/
CSession::~CSession()
{
	
}

/**
@brief 접속 종료를 시도한다.
*/
void CSession::Disconnect()
{
	if(FALSE == isConnected())
	{
		LOG_DEBUG(_T("%s FALSE == isConnected() [%d]"), __TFUNCTION__, GetIdx());
		return;
	}

	if(m_pcIocpInterface)
	{
		m_pcIocpInterface->Disconnect(this);
	}
	else
	{
		LOG_ERROR(_T("%s m_pcIocpInterface is NULL [%d]"), __TFUNCTION__, GetIdx());
	}
}

/**
@brief 소켓이 재사용 되기 위한 초기화 작업을 한다.
*/
void CSession::Initialize()
{
	InterlockedExchange((LONG*)&m_bSending, (LONG)FALSE);

	m_pSendIocpContext->Init();	
	m_pRecvIocpContext->Init();
	m_pConnectIocpContext->Init();
	m_pReuseIocpContext->Init();
}

/**
@brief 소켓을 닫는다. 
@desc 소켓을 닫으면 자동으로 IOCP에 걸려있던 이벤트가 return 된다.
*/
bool CSession::CloseSocket()
{
	return CSocketInterface::CloseSocket();
}

/**
@brief 기준틱과 비교를 통해 현재 틱이 유효한지 검사한다.
@param dwCurTick 현재 틱
@return 유효한지 여부
*/
bool CSession::CheckTick(DWORD dwCurTick)
{
	return m_pcTick->CheckTick(dwCurTick);
}

/**
@brief 기준 틱을 초기화해서 유효 시간을 늘린다.
*/
void CSession::ResetTick()
{
	m_pcTick->ResetTick();
}

/**
@brief 소켓 재사용 가능시 불려지는 메소드
*/
void CSession::OnReuse()
{
	if(m_pcIocpInterface)
		m_pcIocpInterface->OnReuse(this);

	Initialize();
}

/**
@brief WSARecv를 걸어둠으로써, 데이터가 도착했을때 GQCS로 부터 리턴을 받을 수 있게 한다.
@return 성공 여부. 실패하면, 접속 종료 동작을 한다.
*/
bool CSession::Receive()
{
	CIocpContext* pcIocpContext = GetRecvIocpContext();
	if(pcIocpContext == NULL) return false;

	if(FALSE == isConnected()) 
		return false;
	
	DWORD dwRecv = 0, dwFlags = 0;
	int nError = WSARecv(GetSocket(), pcIocpContext->GetWsaBufPtr(), 1, &dwRecv, &dwFlags,  pcIocpContext, NULL);
	if(nError == 0)
	{
		//한번에 받기 성공
		LOG_DEBUG(_T("%s Success [%d] [%d] [%p, %d]"), __TFUNCTION__, GetIdx(), dwRecv, pcIocpContext->GetWsaBufPtr()->buf);
		return true;
	}
	else if(nError == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		//이것도 언젠간 성공이야
		LOG_DEBUG(_T("%s Success IoPending [%d] [%d] [%p, %d]"), __TFUNCTION__, GetIdx(), dwRecv, pcIocpContext->GetWsaBufPtr()->buf);
		return true;
	}
	else
	{
		//실패지만, 정상 동작임.
		LOG_DEBUG(_T("%s Fail [%d, %d] [%d] [%p, %d]"), __TFUNCTION__, GetIdx(), dwRecv, WSAGetLastError(), pcIocpContext->GetWsaBufPtr()->buf, pcIocpContext->GetWsaBufPtr()->len);
		m_pcIocpInterface->OnIoFailed(this);
		return false;
	}
}

/**
@brief WSASend를 걸어둠으로써, 비동기로 데이터를 보낸다. 전송 완료했을때 GQCS로 부터 리턴을 받을 수 있게 한다.
@return 성공 여부. 실패하면, 접속 종료 동작을 한다.
@desc OnSend와 겹치면 안되기에 동기화 필요.
*/
bool CSession::Send()
{
	CScopeCriticalSection lock(m_cSendLock);
	if(TRUE == m_bSending)
		return false;

	if(FALSE == isConnected()) 
		return false;

	CIocpContext* pcIocpContext = GetSendIocpContext();
	if(pcIocpContext == NULL) return false;

	pcIocpContext->Begin();
	if(pcIocpContext->GetWsaBufPtr()->len <= 0) //쌓인 데이터 없으면 false
		return false;

	DWORD dwSend = 0, dwFlags = 0;
	int nError = WSASend(GetSocket(), pcIocpContext->GetWsaBufPtr(), 1, &dwSend, 0, pcIocpContext, NULL);
	if(nError == 0)
	{
		//한번에 보내기 성공
		LOG_DEBUG(_T("%s Success [%d] [%d] [%d]"), __TFUNCTION__, GetIdx(), dwSend);
		m_bSending = TRUE;
		return true;
	} 
	else if(nError == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		//이것도 언젠간 성공이야
		LOG_DEBUG(_T("%s Success. WSA_IO_PENDING [%d] [%d] [%d]"), __TFUNCTION__, GetIdx(), dwSend);
		m_bSending = TRUE;
		return true;
	}
	else
	{
		//실패지만, 정상 동작임.
		LOG_DEBUG(_T("%s Fail [%d] [%d] [%d] [%p]"), __TFUNCTION__, WSAGetLastError(), GetIdx(), dwSend, pcIocpContext->GetWsaBufPtr());
		m_pcIocpInterface->OnIoFailed(this);
		return false;
	}
}

/**
@brief 데이터 전송 완료시 불려지는 메소드. Send 끝난 후 처리와, 필요시 다시 Send를 건다.
@param nLen IO끝난 크기
*/
void CSession::OnSend(unsigned int nLen)
{
	if(FALSE == isConnected())
		return;

	CScopeCriticalSection lock(m_cSendLock);
	if(m_bSending)
	{
		GetSendIocpContext()->Read(nLen);
		GetSendIocpContext()->End();

		m_bSending = FALSE;
		Send();
	}
}

/**
@brief 접속 완료시 불려지는 메소드.
*/
void CSession::OnConnect()
{
	if(m_pcIocpInterface)
		m_pcIocpInterface->OnConnected(this);

	ResetTick(); //접속되면 틱 초기화~

	if(m_pcPeer.Get())
		m_pcPeer->OnConnect();

	LOG_DEBUG(_T("%s [%d]"), __TFUNCTION__, GetIdx());
	Receive();
}

/**
@brief 접속 종료 신호시 불려지는 메소드.
*/
void CSession::OnDisconnect()
{
	if(m_pcIocpInterface)
		m_pcIocpInterface->OnDisconnect(this);
}

/**
@brief 접속 종료 처리를 하는 메소드
*/
void CSession::DisconnectProcess()
{
	InterlockedExchange((LONG *)&m_bConnected, (LONG)FALSE);
	__try
	{
		if(m_pcPeer.Get())
			m_pcPeer->OnDisconnect();
	}
	__except(CMiniDumper::Handler(GetExceptionInformation()))
	{
	}
}

/**
@brief 하나씩 쪼개진 패킷을 푸는 메소드.
*/
void CSession::PacketProcess(CBuffer* pcBuffer)
{
	if(FALSE == isConnected())
		return;

	__try
	{
		m_pcPeer->OnReceive(pcBuffer->GetUsingSize(), pcBuffer->GetHeadBuffer());
	}
	__except(CMiniDumper::Handler(GetExceptionInformation()))
	{
	}
}

/**
@brief Receive 완료시 불려지는 메소드. Peer에게 부탁해 (PacketParse를 통해) 패킷을 한개 단위로 쪼개서 큐에 넣어둔다.
@param nLen Receive된 데이터 길이
@param pData 데이터 포인터
@return 성공 여부. false면 세션을 종료 시킨다.
*/
bool CSession::OnReceive(unsigned int nLen, void* pData)
{
	LOG_DEBUG(_T("%s Receive[%d] Idx [%d]"), __TFUNCTION__, nLen, GetIdx());
	CIocpContext* pcIocpContext = GetRecvIocpContext();
	if(FALSE == isConnected())
		return false;

	if(false == pcIocpContext->Write(nLen))
	{
		LOG_INFO(_T("%s pcIocpContext->Write(%d) failed [%d]"), __TFUNCTION__, nLen, GetIdx());
		Disconnect();
		return false;
	}

	if(true == pcIocpContext->isFull())
	{
		LOG_INFO(_T("%s pcIocpContext->isFull() [%d]"), __TFUNCTION__, GetIdx());
		Disconnect();
		return false;
	}

	while(0 < pcIocpContext->GetUsingSize())
	{
		int nParseLength = m_pcPeer->Verify(pcIocpContext->GetUsingSize(), pcIocpContext->GetHeadBuffer());
		if(nParseLength <= 0) //0이면 패킷이 아직 덜온것
		{
			break;
		}
		else //다른 크기면 유효한걸로 봄.
		{
			CPacketRegister::Register(PACKETEVENT_PACKET, this, new CBuffer(nParseLength, pcIocpContext->GetHeadBuffer()));
			pcIocpContext->Read(nParseLength);
		}
	}
	pcIocpContext->End();
	
	ResetTick(); //무조건 불러준다. 어떤 패킷이 오던 주기적으로 오면 살아있는 것이기에.
	return Receive(); //다음 통지 신호 받고 싶다고 등록
}

/**
@brief Send 요청시 불려지는 메소드. 
@param nLen Receive된 데이터 길이
@param pData 데이터 포인터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@return 성공 여부
*/
bool CSession::Send(const unsigned int nLen, const void* data, const unsigned int nAddLen, const void* pAddData)
{
	CIocpContext* pcIocpContext = GetSendIocpContext();
	if(FALSE == isConnected())
		return false;

	if(!pcIocpContext->AddData(nLen, data, nAddLen, pAddData))
	{
		Disconnect();
		return false;
	}

	return Send();
}

/**
@brief 피어를 지정하는 메소드
@param pcPeer 지정할 피어
*/
void CSession::SetPeer(CPeer* pcPeer)
{
	m_pcPeer = pcPeer;
	if(m_pcPeer.Get())
	{
		m_pcPeer->SetIOInterface(this);
		m_pcPeer->SetSockAddrIn(&m_SockAddrIn);
	}
}


/**
@brief 피어를 제거하는 메소드
*/
void CSession::ClearPeer()
{
	CPeer* pcPeer = m_pcPeer.Release();
	if(pcPeer)
	{
		SAFE_DELETE(pcPeer);
	}
}

/**
@brief 소켓 재사용하고 싶을때 부르는 메소드
*/
void CSession::Reuse()
{
	if(m_pcIocpInterface)
		m_pcIocpInterface->Reuse(this);
}

/**
@brief Connect 플래그 세팅
@param bConnect 접속 여부 플래그
*/
void CSession::SetConnected(BOOL bConnect)
{
	InterlockedExchange((LONG*)&m_bConnected, (LONG)bConnect);
}

/**
@brief 접속/억셉트 시도 실패시
*/
void CSession::ConnectFailed()
{
	if(m_pcIocpInterface)
		m_pcIocpInterface->ConnectFailed(this);
}

} //namespace Redmoon

