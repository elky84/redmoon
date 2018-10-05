#include "Pipe.h"

namespace Redmoon
{

/**
@brief 생성자. 파이프 이름과 피어를 전달 받는다.
@param strName 피어 이름
@param pcPeer IO를 담당할 피어
*/
CPipe::CPipe(const tstring& strName, CPeer* pcPeer) : CIoInterface(pcPeer), m_strName(strName), m_hPipe(INVALID_HANDLE_VALUE), m_bTryConnect(FALSE), m_bConnected(FALSE), 
m_bRefreshing(FALSE), m_cPipeContextAccept(this, IOTYPE_CONNECT), m_cPipeContextRecv(this, IOTYPE_RECV), m_cPipeContextSend(this, IOTYPE_SEND),
m_pcSendBuffer(new CBuffer(PIPE_BUF_SIZE)), m_pcRecvBuffer(new CBuffer(PIPE_BUF_SIZE))
{
	
}

/**
@brief 소멸자.
*/
CPipe::~CPipe()
{
	
}

/**
@brief 비동기 Receive 메시지를 Trigger거는 메소드
@return 성공 여부
*/
bool CPipe::Receive()
{
	if(!isConnected())
		return false;

	DWORD nRecv = 0;
	if(!ReadFile(m_hPipe, m_pcRecvBuffer->GetHeadBuffer(), m_pcRecvBuffer->GetRemainSize(), &nRecv, GetPipeContextRecv()))
	{
		if(ERROR_IO_PENDING != GetLastError())
		{
			LOG_INFO(_T("%s ReadFile Failed [%d] [%p, %d]"), __TFUNCTION__, GetLastError(), m_pcRecvBuffer->GetHeadBuffer(), m_pcRecvBuffer->GetRemainSize());
			return false;
		}
	}
	return true;
}

/**
@brief 재사용 가능해졌을 때 호출하는 메소드
*/
void CPipe::Reuse()
{
	m_bRefreshing = FALSE;	
}

/**
@brief Send 요청시 불려지는 메소드. 
@param nLen Receive된 데이터 길이
@param pData 데이터 포인터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@return 성공 여부
*/
bool CPipe::Send(const unsigned int nLen, const void* pData, const unsigned int nAddLen /* = 0 */, const void* pAddData /* = 0 */)
{
	if(!isConnected())
		return false;

	if(!m_pcSendBuffer->AddData(nLen, pData, nAddLen, pAddData))
	{
		LOG_INFO(_T("%s m_pcSendBuffer->AddData(%d, %p, %d, %p) Failed [%d]"), __TFUNCTION__, nLen, pData, nAddLen, pAddData, GetLastError());
		return false;
	}

	CScopeCriticalSection cs(m_cSendLock);
	DWORD nWritten = 0;
	if(!WriteFile(m_hPipe, m_pcSendBuffer->GetHeadBuffer(), m_pcSendBuffer->GetUsingSize(), &nWritten, GetPipeContextSend()))
	{
		if(ERROR_IO_PENDING != GetLastError())
		{
			LOG_INFO(_T("%s WriteFile(%d, %d, %d) Failed [%d]"), __TFUNCTION__, m_hPipe, m_pcSendBuffer->GetHeadBuffer(), m_pcSendBuffer->GetUsingSize(), GetLastError());
			return false;
		}
	}
	return true;
}

/**
@brief Event 받을 대상으로 등록 요청
*/
void CPipe::Register()
{
	GetPipeContextAccept()->Register();
	GetPipeContextRecv()->Register();
	GetPipeContextSend()->Register();
}

/**
@brief Receive 완료시 이벤트 메소드
@param nLen Receive 온 크기
@param pData 데이터 포인터
@return 성공 여부
*/
bool CPipe::OnReceive(unsigned int nLen, void* pData)
{
	m_pcRecvBuffer->JumpTail(nLen);
	while(0 < m_pcRecvBuffer->GetUsingSize())
	{
		int nPacketLen = m_pcPeer->Verify(m_pcRecvBuffer->GetUsingSize(), m_pcRecvBuffer->GetHeadBuffer());
		if(nPacketLen <= 0)
		{
			Disconnect();
			return 0;
		}

		m_pcPeer->OnReceive(nPacketLen, m_pcRecvBuffer->GetHeadBuffer());
		m_pcRecvBuffer->JumpHead(nPacketLen);
	}
	m_pcRecvBuffer->Compress();
	Receive();
	return true;
}

/**
@brief 접속 성공시 이벤트 메소드
*/
void CPipe::OnConnect()
{
	m_pcPeer->OnConnect();
	m_bConnected = TRUE;
	Receive();
}

/**
@brief 접속 해제시 이벤트 메소드
*/
void CPipe::OnDisconnect()
{
	m_bConnected = FALSE;
	m_bRefreshing = FALSE;

	m_pcPeer->OnDisconnect();

	m_pcRecvBuffer->Init();
	m_pcSendBuffer->Init();
}

/**
@brief Send완료시 이벤트 메소드
*/
void CPipe::OnSend(unsigned int nLen)
{
	CScopeCriticalSection cs(m_cSendLock);
	m_pcSendBuffer->JumpHead(nLen);
	m_pcSendBuffer->Compress();
}

} //namespace Redmoon